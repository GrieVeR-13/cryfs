#ifndef EDS3_STREAMBUFTEST_H
#define EDS3_STREAMBUFTEST_H


class MyBuffer : public std::basic_streambuf<char> {
private:
    char inbuf[10];
    char outbuf[10];

public:
    MyBuffer() {
        setg(inbuf+4, inbuf+4, inbuf+4);
        setp(outbuf, outbuf+9);
    }

    int sync() override {
        int return_code = 0;

        for (int i = 0; i < (pptr() - pbase()); i++) {
            if (std::putchar(outbuf[i]) == EOF) {
                return_code = EOF;
                break;
            }
        }

        pbump(pbase() - pptr());
        return return_code;
    }

    int_type overflow(int_type ch) override {
        *pptr() = ch;
        pbump(1);

        return (sync() == EOF ? EOF : ch);
    }

    int_type underflow() override {
//            int keep = std::max(long(4), (gptr() - eback()));
        int keep = 0;

        std::memmove(inbuf + 4 - keep, gptr() - keep, keep);

        int read = 0;
        int ch, position = 4;
        while ((ch = std::getchar()) != EOF && position <= 10) {
            inbuf[position++] = char(ch);
            read++;
        }

        if (read == 0) return EOF;
        setg(inbuf - keep + 4, inbuf + 4 , inbuf + position);
        return *gptr();
    }
};


class compressbuf : public std::streambuf {
    char*           buffer_;
public:
    compressbuf()
            : buffer_(new char[1024]) {
        // initialize compression context
    }

    int underflow() override {
        if (this->gptr() == this->egptr()) {
            // decompress data into buffer_, obtaining its own input from
            // this->sbuf_; if necessary resize buffer
            // the next statement assumes "size" characters were produced (if
            // no more characters are available, size == 0.
            this->setg(this->buffer_, this->buffer_, this->buffer_ + 0);
        }
        return this->gptr() == this->egptr()
               ? std::char_traits<char>::eof()
               : std::char_traits<char>::to_int_type(*this->gptr());
    }

    virtual std::streambuf::int_type overflow(std::streambuf::int_type value) override {
        return std::streambuf::overflow(value);
    }


    virtual int sync() override
    {
        return std::streambuf::sync();
    }
};

template <typename Byte = char>
class istreambuf_view : public std::streambuf
{
public:
    using byte = Byte;
    static_assert(1 == sizeof(byte), "sizeof buffer element type 1.");

    istreambuf_view(const byte* data, size_t len) :     // ptr + size
            begin_(data), end_(data + len), current_(data)
    {}

    istreambuf_view(const byte* beg, const byte* end) : // begin + end
            begin_(beg), end_(end), current_(beg)
    {}

protected:
    int_type underflow() override
    {
        return (current_ == end_ ? traits_type::eof() : traits_type::to_int_type(*current_));
    }

    int_type uflow() override
    {
        return (current_ == end_ ? traits_type::eof() : traits_type::to_int_type(*current_++));
    }

    int_type pbackfail(int_type ch) override
    {
        if (current_ == begin_ || (ch != traits_type::eof() && ch != current_[-1]))
            return traits_type::eof();

        return traits_type::to_int_type(*--current_);
    }

    std::streamsize showmanyc() override
    {
        return end_ - current_;
    }

    const byte* const begin_;
    const byte* const end_;
    const byte* current_;
};

class FILE_buffer2 : public std::streambuf
{
public:
    explicit FILE_buffer2(FILE *fptr, std::size_t buff_sz = 256, std::size_t put_back = 8);

private:
    //overrides base class underflow()
    int_type underflow() override;

    //copy ctor and assignment not implemented;
    //copying not allowed
    FILE_buffer2(const FILE_buffer2 &);
    FILE_buffer2 &operator= (const FILE_buffer2 &);

private:
    FILE *fptr_;
    const std::size_t put_back_;
    std::vector<char> buffer_;
};

FILE_buffer2::FILE_buffer2(FILE *fptr, size_t buff_sz, size_t put_back) :
        fptr_(fptr),
        put_back_(std::max(put_back, size_t(1))),
        buffer_(std::max(buff_sz, put_back_) + put_back_)
{
    char *end = &buffer_.front() + buffer_.size();
    setg(end, end, end);
}

std::streambuf::int_type FILE_buffer2::underflow()
{
    if (gptr() < egptr()) //buffer not exhausted
        return traits_type::to_int_type(*gptr());

    char *base = &buffer_.front();
    char *start = base;

    if (eback() == base) //true when this isn't the first fill
    {
        //Make arrangements for putback characters
        std::memmove(base, egptr() - put_back_, put_back_);
        start += put_back_;
    }

    //start is now the start of the buffer, proper.
    //Read from fptr_ in to the provided buffer
//        size_t n = fptr_->read((uint8_t *)start, buffer_.size() - (start - base));
    size_t n = std::fread(start, 1, buffer_.size() - (start - base), fptr_);
    if (n == 0)
        return traits_type::eof();

    //Set buffer pointers
    setg(base, start, start + n);

    return traits_type::to_int_type(*gptr());
}

class StreamBufTest {

    static void test() {
        {
//                auto buffer = "TEST 42";
//                auto view_buf = istreambuf_view<>(buffer.data(), buffer.size());
//                std::istream istr(&view_buf);
//
//                std::string str;
//                int v = 0;
//                istr >> str >> v; // Read string and then integer
//                assert("TEST" == str && 42 == v);

        }


        auto isObject = options.baseDir().getDataFileSystem()->openInputStream("/test");
        RandomAccessIONative randomAccessIoNative(isObject);


        auto cbsbuf = make_callback_ostreambuf([](const void* buf, std::streamsize sz, void* user_data)
                                               {
                                                   RandomAccessIONative *randomAccessIoNative2 = (RandomAccessIONative *)user_data;
                                                   randomAccessIoNative2->write((uint8_t *)buf, sz);
//                                                       std::cout.write(reinterpret_cast<const char*>(buf), sz);

                                                   return sz; // return the numbers of characters written.
                                               }, &randomAccessIoNative);
//            std::ostream ostr(&cbsbuf);
//            ostr << "q"; // Write string and integer
//            randomAccessIoNative.close();



//            compressbuf   sbuf;
//            std::istream  in(&sbuf);

//            std::ifstream file("/storage/emulated/0/1.txt");
//            if (!file.good()) {
//                LOGI("asd");
//            }
//            std::FILE* f = std::fopen("/storage/emulated/0/AndroidManifest.xml", "r");
        std::FILE* f = std::fopen("/storage/emulated/0/Android/data/com.sovworks.projecteds3/files/cryfsTestDir/sourceDir/test", "r");
        FILE_buffer2 fbuf2(f);
        FILE_buffer fbuf(&randomAccessIoNative);
        std::istream  in(&fbuf);
//            uint32_t value = 0;
//            in >> value;
        std::string str;
        in >> str;
        in >> str;
        in >> str;
        in >> str;
//            in.close();
    }
};


#endif //EDS3_STREAMBUFTEST_H
