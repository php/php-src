--TEST--
Test session_encode() function : Numeric key raise error. bug65359
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
ob_start();

ini_set('session.serialize_handler', 'php_serialize');
var_dump(session_start());
$_SESSION[-3] = 'foo';
$_SESSION[3] = 'bar';
$_SESSION['var'] = 123;
var_dump(session_encode());
session_write_close();

// Should finish without errors
echo 'Done'.PHP_EOL;
?>
--EXPECT--
bool(true)
string(51) "a:3:{i:-3;s:3:"foo";i:3;s:3:"bar";s:3:"var";i:123;}"
Done
