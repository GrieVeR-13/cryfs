#include <filesystem/RandomAccessIONative.h>
#include <util.h>
#include <nativehelper/ScopedLocalRef.h>
#include <Exception.h>
#include "EdsDataFileSystem.h"

namespace cpputils {

    template<typename Callback>
    struct CallbackOutputStreamBuf : public std::streambuf {
        using callback_t = Callback;

        CallbackOutputStreamBuf(Callback cb, void *user_data = nullptr) :
                callback_(cb), user_data_(user_data) {}

    protected:
        std::streamsize xsputn(const char_type *s, std::streamsize n) override {
            return callback_(s, n, user_data_); // returns the number of characters successfully written.
        };

        int_type overflow(int_type ch) override {
            return callback_(&ch, 1, user_data_); // returns the number of characters successfully written.
        }

    private:
        Callback callback_;
        void *user_data_;
    };

    template<typename Callback>
    auto makeCallbackOutputStreamBuf(Callback cb, void *user_data = nullptr) {
        return new CallbackOutputStreamBuf<Callback>(cb, user_data);
    }


    class RandomAccessIONativeStreamBuf : public std::streambuf {
    public:
        explicit RandomAccessIONativeStreamBuf(RandomAccessIONative *randomAccessIoNative, std::size_t buff_sz = 256,
                                               std::size_t put_back = 8) :
                randomAccessIoNative(randomAccessIoNative),
                putBack(std::max(put_back, size_t(1))),
                buffer(std::max(buff_sz, putBack) + putBack) {
            char *end = &buffer.front() + buffer.size();
            setg(end, end, end);
        }

    private:
        int_type underflow() override {
            if (gptr() < egptr())
                return traits_type::to_int_type(*gptr());

            char *base = &buffer.front();
            char *start = base;

            if (eback() == base) {
                std::memmove(base, egptr() - putBack, putBack);
                start += putBack;
            }

            int n = randomAccessIoNative->read(reinterpret_cast<uint8_t *>(start), buffer.size() - (start - base));
            if (n == -1)
                return traits_type::eof();
            setg(base, start, start + n);

            return traits_type::to_int_type(*gptr());
        }

        RandomAccessIONativeStreamBuf(const RandomAccessIONativeStreamBuf &);

        RandomAccessIONativeStreamBuf &operator=(const RandomAccessIONativeStreamBuf &);

    private:
        RandomAccessIONative *randomAccessIoNative;
        const std::size_t putBack;
        std::vector<char> buffer;
    };


    class InputStreamNativeIStream : public std::basic_istream<char> {
    private:
        RandomAccessIONative randomAccessIoNative;
        RandomAccessIONativeStreamBuf streamBuf;
    public:
        InputStreamNativeIStream(jobject inputStreamObject) : std::basic_istream<char>(&streamBuf),
                                                              randomAccessIoNative(inputStreamObject, true),
                                                              streamBuf(&randomAccessIoNative) {

        }

        ~InputStreamNativeIStream() {
            randomAccessIoNative.close();
        }

    };

    class OutputStreamNativeOStream : public std::basic_ostream<char> {
    private:
        std::unique_ptr<RandomAccessIONative> randomAccessIoNative;
        std::unique_ptr<std::streambuf> streamBuf;
    public:
        OutputStreamNativeOStream(jobject outputStreamObject) : OutputStreamNativeOStream(new RandomAccessIONative(outputStreamObject, true)) {}

        ~OutputStreamNativeOStream() {
            randomAccessIoNative->close();
        }

    private:
        OutputStreamNativeOStream(RandomAccessIONative *randomAccessIoNative, std::streambuf *streamBuf)
                : std::basic_ostream<char>(streamBuf),
                  randomAccessIoNative(randomAccessIoNative), streamBuf(streamBuf) {
        }

        OutputStreamNativeOStream(RandomAccessIONative *randomAccessIoNative) :
                OutputStreamNativeOStream(randomAccessIoNative,
                                          makeCallbackOutputStreamBuf(
                                                 [](const void *buf, std::streamsize size, void *user_data) {
                                                     auto randomAccessIoNative = static_cast<RandomAccessIONative *>(user_data);
                                                     randomAccessIoNative->write((uint8_t *) buf, size);
                                                     return size;
                                                 }, randomAccessIoNative)
                ) {}
    };

    bool EdsDataFileSystem::exists(const boost::filesystem::path &path) const {
        return pathnameFileSystemNative->exists(path.string());
    }

    void EdsDataFileSystem::create_directories(const boost::filesystem::path &path) const {
        pathnameFileSystemNative->newGroup(path.string(), true);
    }


    void EdsDataFileSystem::remove(const boost::filesystem::path &path) const {
        pathnameFileSystemNative->deleteObject(path.string());
    }

    std::unique_ptr<std::istream> EdsDataFileSystem::openInputStream(const boost::filesystem::path &path) const {  //todoe std::ios::binary ?
        try {
            auto inputStreamObject = ScopedLocalRef<jobject>(get_env(), pathnameFileSystemNative->openRandomAccessIO(path.string()));
            return std::make_unique<InputStreamNativeIStream>(inputStreamObject.get());
        }
        catch(const Exception &e) {
            return std::make_unique<std::basic_istream<char>>(nullptr);
        }

    }

    std::unique_ptr<std::ostream> EdsDataFileSystem::openOutputStream(const boost::filesystem::path &path) const { //todoe std::ios::binary | std::ios::trunc ?
        try {
            if (!pathnameFileSystemNative->exists(path.string())) {
                pathnameFileSystemNative->newFile(path.string());
            }
            auto outputStreamObject = ScopedLocalRef<jobject>(get_env(), pathnameFileSystemNative->openRandomAccessIO(path.string()));
            return std::make_unique<OutputStreamNativeOStream>(outputStreamObject.get());
        }
        catch(const Exception &e) {
            return std::make_unique<std::basic_ostream<char>>(nullptr);
        }
    }
}
