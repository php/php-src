--TEST--
Test octdec() with number larger than 32 bit (32bit platform)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php

echo "octdec('020000000000') should be 2147483648 is ";
$res = octdec('020000000000');
var_dump($res);
echo "octdec('20000000000') should be 2147483648 is ";
$res = octdec('20000000000');
var_dump($res);

?>
--EXPECT--
octdec('020000000000') should be 2147483648 is float(2147483648)
octdec('20000000000') should be 2147483648 is float(2147483648)
