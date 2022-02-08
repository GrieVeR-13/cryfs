#ifndef FUSE_EDS_H_
#define FUSE_EDS_H_

#include "FuseCommonEds.h"

#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/uio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fuse;

enum fuse_readdir_flags {
    FUSE_READDIR_PLUS = (1 << 0)
};

enum fuse_fill_dir_flags {
    FUSE_FILL_DIR_PLUS = (1 << 1)
};

typedef int (*fuse_fill_dir_t)(void *buf, const char *name,
                               const struct stat *stbuf, off_t off,
                               enum fuse_fill_dir_flags flags);

struct fuse_config {
    int set_gid;
    unsigned int gid;

    int set_uid;
    unsigned int uid;

    int set_mode;
    unsigned int umask;

    double entry_timeout;

    double negative_timeout;

    double attr_timeout;

    int intr;

    int intr_signal;

    int remember;

    int hard_remove;

    int use_ino;

    int readdir_ino;

    int direct_io;

    int kernel_cache;

    int auto_cache;

    int ac_attr_timeout_set;
    double ac_attr_timeout;

    int nullpath_ok;

    int show_help;
    char *modules;
    int debug;
};


struct fuse_operations {

    int (*getattr)(const char *, struct stat *);

    int (*fgetattr)(const char *, struct stat *, struct fuse_file_info *);

    int (*readlink)(const char *, char *, size_t);

    int (*mknod)(const char *, mode_t, dev_t);

    int (*mkdir)(const char *, mode_t);

    int (*unlink)(const char *);

    int (*rmdir)(const char *);

    int (*symlink)(const char *, const char *);

    int (*rename)(const char *, const char *);

    int (*link)(const char *, const char *);

    int (*chmod)(const char *, mode_t);

    int (*chown)(const char *, uid_t, gid_t);

    int (*truncate)(const char *, int64_t);

    int (*ftruncate)(const char *, int64_t, struct fuse_file_info *);

    int (*open)(const char *, struct fuse_file_info *);

    int (*read)(const char *, char *, size_t, int64_t, struct fuse_file_info *);

    int (*write)(const char *, const char *, size_t, int64_t, struct fuse_file_info *);

    int (*statfs)(const char *, struct statvfs *);

    int (*flush)(const char *, struct fuse_file_info *);

    int (*release)(const char *, struct fuse_file_info *);

    int (*fsync)(const char *, int, struct fuse_file_info *);

    int (*setxattr)(const char *, const char *, const char *, size_t, int);

    int (*getxattr)(const char *, const char *, char *, size_t);

    int (*listxattr)(const char *, char *, size_t);

    int (*removexattr)(const char *, const char *);

    int (*opendir)(const char *, struct fuse_file_info *);

    int (*readdir)(const char *, void *, fuse_fill_dir_t, int64_t, struct fuse_file_info *);

    int (*releasedir)(const char *, struct fuse_file_info *);

    int (*fsyncdir)(const char *, int, struct fuse_file_info *);

    void *(*init)(struct fuse_conn_info *conn);

    void (*destroy)(void *private_data);

    int (*access)(const char *, int);

    int (*create)(const char *, mode_t, struct fuse_file_info *);

    int (*lock)(const char *, struct fuse_file_info *, int cmd,
                struct flock *);

    int (*utimens)(const char *, const struct timespec tv[2]);

    int (*bmap)(const char *, size_t blocksize, uint64_t *idx);

    int (*ioctl)(const char *, int cmd, void *arg,
                 struct fuse_file_info *, unsigned int flags, void *data);

    int (*poll)(const char *, struct fuse_file_info *,
                struct fuse_pollhandle *ph, unsigned *reventsp);

    int (*write_buf)(const char *, struct fuse_bufvec *buf, off_t off,
                     struct fuse_file_info *);

    int (*read_buf)(const char *, struct fuse_bufvec **bufp,
                    size_t size, off_t off, struct fuse_file_info *);

    int (*flock)(const char *, struct fuse_file_info *, int op);

    int (*fallocate)(const char *, int, off_t, off_t,
                     struct fuse_file_info *);

    ssize_t (*copy_file_range)(const char *path_in,
                               struct fuse_file_info *fi_in,
                               off_t offset_in, const char *path_out,
                               struct fuse_file_info *fi_out,
                               off_t offset_out, size_t size, int flags);

    off_t (*lseek)(const char *, off_t off, int whence, struct fuse_file_info *);
};

struct fuse_context {
    struct fuse *fuse;

    uid_t uid;

    gid_t gid;

    pid_t pid;

    void *private_data;

    mode_t umask;
};

#define fuse_main(argc, argv, op, private_data)                \
    fuse_main_real(argc, argv, op, sizeof(*(op)), private_data)


struct fuse *fuse_new(struct fuse_args *args, const struct fuse_operations *op,
                      size_t op_size, void *private_data);

int fuse_mount(struct fuse *f, const char *mountpoint);

void fuse_unmount(struct fuse *f);

void fuse_destroy(struct fuse *f);

int fuse_loop(struct fuse *f);

void fuse_exit(struct fuse *f);

int fuse_loop_mt_31(struct fuse *f, int clone_fd);

struct fuse_context *fuse_get_context(void);

int fuse_getgroups(int size, gid_t list[]);

int fuse_interrupted(void);


int fuse_invalidate_path(struct fuse *f, const char *path);

int fuse_main_real(int argc, char *argv[], const struct fuse_operations *op,
                   size_t op_size, void *private_data);

int fuse_main_mount(int argc, char *argv[], const struct fuse_operations *op,
                    size_t op_size, void *user_data, struct fuse **fuse_result);

int fuse_main_loop(struct fuse *fuse);

void fuse_main_unmount(struct fuse *fuse);

int fuse_start_cleanup_thread(struct fuse *fuse);

void fuse_stop_cleanup_thread(struct fuse *fuse);

int fuse_clean_cache(struct fuse *fuse);

struct fuse_fs;

struct fuse_fs *fuse_fs_new(const struct fuse_operations *op, size_t op_size,
                            void *private_data);

typedef struct fuse_fs *(*fuse_module_factory_t)(struct fuse_args *args,
                                                 struct fuse_fs *fs[]);


struct fuse_session *fuse_get_session(struct fuse *f);

int fuse_open_channel(const char *mountpoint, const char *options);

#ifdef __cplusplus
}
#endif

#endif
