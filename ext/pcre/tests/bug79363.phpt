--TEST--
Bug #79363 (\p{L} doesn't work alongside \p{Arabic} in a character class)
--FILE--
<?php
$str = 'lower UPPER';
var_dump(preg_replace('/[\p{L}\p{Arabic}]/', '0', $str));
var_dump(preg_replace('/[^\p{L}\p{Arabic}]/', '0', $str));
?>
--EXPECT--
string(11) "00000 00000"
string(11) "lower0UPPER"
