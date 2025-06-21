--TEST--
bindec() with number larger than 32 bit (64bit platform)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--INI--
precision=14
--FILE--
<?php
$value = '10000000000000000000000000000000';

echo "bindec('0b10000000000000000000000000000000') should be 2147483648 is ";
$res = bindec('0b10000000000000000000000000000000');
var_dump($res);
echo "bindec('10000000000000000000000000000000') should be 2147483648 is ";
$res = bindec('10000000000000000000000000000000');
var_dump($res);

?>
--EXPECT--
bindec('0b10000000000000000000000000000000') should be 2147483648 is int(2147483648)
bindec('10000000000000000000000000000000') should be 2147483648 is int(2147483648)
