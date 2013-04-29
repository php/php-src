--TEST--
strval() function
--FILE--
<?php
$foo = 'bar';
var_dump(strval($foo));
define('FOO', 'BAR');
var_dump(strval(FOO));
var_dump(strval('foobar'));
var_dump(strval(1));
var_dump(strval(1.1));
var_dump(strval(true));
var_dump(strval(false));
var_dump(strval(array('foo')));
?>
--EXPECTF--
string(3) "bar"
string(3) "BAR"
string(6) "foobar"
string(1) "1"
string(3) "1.1"
string(1) "1"
string(0) ""

Notice: Array to string conversion in %sstrval.php on line %d
string(5) "Array"
