--TEST--
Bug #61764: 'I' unpacks n as signed if n > 2^31-1 on LP64
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
//expected -30000 mod 2^32 = 4294937296, and not -30000
//because we can represent 4294937296 with our PHP int type
print_r(unpack('I', pack('L', -30000)));
?>
--EXPECT--
Array
(
    [1] => 4294937296
)
