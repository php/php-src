--TEST--
FFI 400: Call each other type struct of multiple FFI instance
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--INI--
ffi.enable=1
--FILE--
<?php
$cdef = '
typedef struct _php_stream_notifier php_stream_notifier;
typedef struct _php_stream_context php_stream_context;
typedef void (*php_stream_notification_func)(php_stream_context *context,
                int notifycode, int severity,
                char *xmsg, int xcode,
                size_t bytes_sofar, size_t bytes_max,
                void * ptr);
struct _php_stream_notifier {
    php_stream_notification_func func;
    void (*dtor)(php_stream_notifier *notifier);
    void* ptr;
    int mask;
    size_t progress, progress_max; /* position for progress notification */
};

struct _php_stream_context {
    php_stream_notifier *notifier;
    void* options;   /* hash keyed by wrapper family or specific wrapper */
    void *res;     /* used for auto-cleanup */
};

extern void php_stream_context_free(php_stream_context *context);
extern php_stream_context *php_stream_context_alloc(void);
';

$ffi1 = FFI::cdef($cdef);
$ffi2 = FFI::cdef($cdef);
$c1 = $ffi1->new('php_stream_context', false);
$c2 = $ffi1->php_stream_context_alloc();
FFI::memcmp($c2[0], $c2, FFI::sizeof($c1));

try {
    $tz = $ffi2->php_stream_context_free(FFI::addr($c1));
    echo 'Success';
} catch (FFI\Exception $e) {
    echo 'incompatible';
}
?>
--EXPECT--
Success
