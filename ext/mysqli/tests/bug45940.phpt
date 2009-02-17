--TEST--
Bug #45940 MySQLI OO does not populate connect_error property on failed connect
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "connect.inc";

//a forced username/password mismatch
$dbo = @new mysqli($host, 'hopenotexist', 'andifheexistshehasanotherpassword', 'my_db');

var_dump($dbo->connect_error);
var_dump(mysqli_connect_error());
?>
--EXPECTF--
string(71) "Access denied for user 'hopenotexist'@'localhost' (using password: YES)"
string(71) "Access denied for user 'hopenotexist'@'localhost' (using password: YES)"

