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
unicode(3) "bar"
unicode(3) "BAR"
unicode(6) "foobar"
unicode(1) "1"
unicode(3) "1.1"
unicode(1) "1"
unicode(0) ""

Notice: Array to string conversion in %sstrval.php on line 11
unicode(5) "Array"
