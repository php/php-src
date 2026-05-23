#ifdef __FreeBSD__
   
   #include "php_io_internal.h"
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <sys/uio.h>
   #include <unistd.h>
   #include <errno.h>
   
   static ssize_t php_io_freebsd_sendfile(int src_fd, int dest_fd, size_t maxlen)
   {
       off_t total_sent = 0;
       size_t remaining = (maxlen == PHP_IO_COPY_ALL) ? 0 : maxlen;
   
       while (maxlen == PHP_IO_COPY_ALL || remaining > 0) {
           off_t sent_in_this_call = 0;
           int result = sendfile(src_fd, dest_fd, total_sent, remaining, NULL, &sent_in_this_call, 0);
   
           if (sent_in_this_call > 0) {
               total_sent += sent_in_this_call;
               if (maxlen != PHP_IO_COPY_ALL) {
                   remaining -= (size_t)sent_in_this_call;
               }
           }
   
           if (result == 0) {
               if (sent_in_this_call == 0 || remaining == 0) {
                   break;
               }
           } else {
               if (errno == EAGAIN || errno == EINTR) {
                   if (sent_in_this_call > 0) {
			   continue;
		   }
                   break;
               }
               if (total_sent == 0) {
                   return php_io_generic_copy_fallback(src_fd, dest_fd, maxlen);
               }
               break;
           }
       }
   
       return (total_sent > 0) ? (ssize_t)total_sent : -1;
   }
   
   ssize_t php_io_freebsd_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen)
   {
       /* unlike linux, sendfile on freebsd works only under those conditions */
       if (src->fd_type == PHP_IO_FD_FILE && dest->fd_type == PHP_IO_FD_SOCKET) {
           return php_io_freebsd_sendfile(src->fd, dest->fd, maxlen);
       }
   
       return php_io_generic_copy_fallback(src->fd, dest->fd, maxlen);
   }
   
   #endif

