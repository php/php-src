--TEST--
Interface of the class mysqli_driver - Reflection
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('connect.inc');
if (($tmp = substr(PHP_VERSION, 0, strpos(PHP_VERSION, '.'))) && ($tmp < 5))
	die("skip Reflection not available before PHP 5 (found PHP $tmp)");

/*
Let's not deal with cross-version issues in the EXPECTF/UEXPECTF.
Most of the things which we test are covered by mysqli_class_*_interface.phpt.
Those tests go into the details and are aimed to be a development tool, no more.
*/
if (!$IS_MYSQLND)
	die("skip Test has been written for the latest version of mysqlnd only");
if ($MYSQLND_VERSION < 576)
	die("skip Test requires mysqlnd Revision 576 or newer");
?>
--FILE--
<?php
	require_once('reflection_tools.inc');
	$class = new ReflectionClass('mysqli_driver');
	inspectClass($class);
	print "done!";
?>
--EXPECTF--
Inspecting class 'mysqli_driver'
isInternal: yes
isUserDefined: no
isInstantiable: yes
isInterface: no
isAbstract: no
isFinal: yes
isIteratable: no
Modifiers: '%d'
Parent Class: ''
Extension: 'mysqli'
done!