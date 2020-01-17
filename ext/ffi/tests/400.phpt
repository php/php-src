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
typedef struct _zend_file_handle {
    union {
            void*         *fp;
            void*   stream;
    } handle;
    const char        *filename;
    void*       *opened_path;
    void*  type;
    /* free_filename is used by wincache */
    /* TODO: Clean up filename vs opened_path mess */
    void*         free_filename;
    char              *buf;
    size_t            len;
} zend_file_handle;
void zend_stream_init_filename(zend_file_handle *handle, const char *filename);
';

$ffi1 = FFI::cdef($cdef);
$ffi2 = FFI::cdef($cdef);
$c1 = $ffi1->new('zend_file_handle');
try {
    $ffi2->zend_stream_init_filename(FFI::addr($c1), __FILE__);
    echo 'Success';
} catch (FFI\Exception $e) {
    echo 'incompatible';
}

?>
--EXPECT--
Success
