--TEST--
JIT INLINE DO_ICALL: 002
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
function test(string $x) {
    return ord($x) + 1000;
}
echo test(''), "\n";
echo test("\xff"), "\n";
echo test("0123456789"), "\n";
$i = 99;
echo ord($i), "\n";  // not inlined, $i converted to "9" and the ordinal value is fetched
?>
--EXPECT--
1000
1255
1048
57
