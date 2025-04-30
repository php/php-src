--TEST--
GH-16355 (Assertion failure in ext/opcache/jit/ir/ir_ra.c:1139)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1203
opcache.jit_buffer_size=64M
--FILE--
<?php
function test($in, $cond) {
    if ($cond) {
        eval('');
    }
    while (stream_bucket_make_writeable($in)) {}
}
?>
DONE
--EXPECT--
DONE
