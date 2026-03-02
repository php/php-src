#ifndef PHP_IO_WINDOWS_H
#define PHP_IO_WINDOWS_H

/* Copy operations */
ssize_t php_io_windows_copy_file_to_file(int src_fd, int dest_fd, size_t maxlen);
ssize_t php_io_windows_copy_file_to_generic(int src_fd, int dest_fd, size_t maxlen);
ssize_t php_io_windows_copy_generic_to_file(int src_fd, int dest_fd, size_t maxlen);
ssize_t php_io_windows_copy_generic_to_generic(int src_fd, int dest_fd, size_t maxlen);

/* Instance initialization macros */
#define PHP_IO_PLATFORM_COPY_OPS \
	{ \
		.file_to_file = php_io_windows_copy_file_to_file, \
		.file_to_generic = php_io_windows_copy_file_to_generic, \
		.generic_to_file = php_io_windows_copy_generic_to_file, \
		.generic_to_generic = php_io_windows_copy_generic_to_generic, \
	}

#define PHP_IO_PLATFORM_NAME "windows"

#endif /* PHP_IO_WINDOWS_H */
