--TEST--
Bug #79576 ("TYPE *" shows unhelpful message when type is not defined)
--SKIPIF--
<?php
if (!extension_loaded('ffi')) die('skip ffi extension not available');
if (PHP_DEBUG || getenv('SKIP_ASAN')) echo "xfail: FFI cleanup after parser error is nor implemented";
?>
--FILE--
<?php
try {
	FFI::cdef('struct tree *get_tree(const oid *, size_t, struct tree *);');
} catch (Throwable $e) {
	echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
	FFI::cdef('struct tree *get_tree(oid, size_t, struct tree *);');
} catch (Throwable $e) {
	echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
	FFI::cdef('
typedef struct _simple_struct {
        const some_not_declared_type **property;
    } simple_struct;
');
} catch (Throwable $e) {
	echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
DONE
--EXPECT--
FFI\ParserException: undefined C type 'oid' at line 1
FFI\ParserException: undefined C type 'oid' at line 1
FFI\ParserException: undefined C type 'some_not_declared_type' at line 3
DONE
