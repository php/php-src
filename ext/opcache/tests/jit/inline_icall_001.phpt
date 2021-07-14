--TEST--
JIT INLINE DO_ICALL: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
;opcache.jit_debug=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test(object $x) {
    return spl_object_id($x) + 100;
}
$a = new stdClass();
$b = new stdClass();
echo test($a), "\n";
echo test($b), "\n";
echo spl_object_id(new stdClass()), "\n";  // not inlined
echo "\n";
?>
--EXPECT--
101
102
3
