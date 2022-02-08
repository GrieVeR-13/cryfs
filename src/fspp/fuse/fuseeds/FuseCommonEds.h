#ifndef FUSE_COMMON_H_
#define FUSE_COMMON_H_

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fuse_file_info {
    int flags;

    unsigned int writepage: 1;

    unsigned int direct_io: 1;

    unsigned int keep_cache: 1;

    unsigned int flush: 1;

    unsigned int nonseekable: 1;

    unsigned int flock_release: 1;

    unsigned int cache_readdir: 1;

    unsigned int padding: 25;
    unsigned int padding2: 32;

    uint64_t fh;

    uint64_t lock_owner;

    uint32_t poll_events;
};

struct fuse_loop_config {

    int clone_fd;

    unsigned int max_idle_threads;
};

struct fuse_conn_info {
    unsigned proto_major;

    unsigned proto_minor;

    unsigned max_write;


    unsigned max_read;

    unsigned max_readahead;

    unsigned capable;

    unsigned want;

    unsigned max_background;

    unsigned congestion_threshold;

    unsigned time_gran;

    unsigned reserved[22];
};

struct fuse_session;
struct fuse_pollhandle;
struct fuse_conn_info_opts;

struct fuse_conn_info_opts *fuse_parse_conn_info_opts(struct fuse_args *args);

void fuse_apply_conn_info_opts(struct fuse_conn_info_opts *opts,
                               struct fuse_conn_info *conn);


int fuse_daemonize(int foreground);

int fuse_version(void);

const char *fuse_pkgversion(void);

void fuse_pollhandle_destroy(struct fuse_pollhandle *ph);

enum fuse_buf_flags {
    FUSE_BUF_IS_FD = (1 << 1),


    FUSE_BUF_FD_SEEK = (1 << 2),

    FUSE_BUF_FD_RETRY = (1 << 3)
};

enum fuse_buf_copy_flags {

    FUSE_BUF_NO_SPLICE = (1 << 1),

    FUSE_BUF_FORCE_SPLICE = (1 << 2),

    FUSE_BUF_SPLICE_MOVE = (1 << 3),

    FUSE_BUF_SPLICE_NONBLOCK = (1 << 4)
};

struct fuse_buf {
    size_t size;

    enum fuse_buf_flags flags;

    void *mem;

    int fd;
    off_t pos;
};

struct fuse_bufvec {
    size_t count;

    size_t idx;

    size_t off;

    struct fuse_buf buf[1];
};

size_t fuse_buf_size(const struct fuse_bufvec *bufv);


ssize_t fuse_buf_copy(struct fuse_bufvec *dst, struct fuse_bufvec *src,
                      enum fuse_buf_copy_flags flags);

int fuse_set_signal_handlers(struct fuse_session *se);


void fuse_remove_signal_handlers(struct fuse_session *se);

#ifdef __cplusplus
}
#endif


#endif /* FUSE_COMMON_H_ */
