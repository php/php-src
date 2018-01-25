--TEST--
strtr() with references
--FILE--
<?php

$foo = 'foo';
$arr = ['bar' => &$foo];
var_dump(strtr('foobar', $arr));

?>
--EXPECT--
string(6) "foofoo"
