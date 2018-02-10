--TEST--
Interface of the class mysqli_driver - Reflection
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('connect.inc');

/*
Let's not deal with cross-version issues in the EXPECTF/UEXPECTF.
Most of the things which we test are covered by mysqli_class_*_interface.phpt.
Those tests go into the details and are aimed to be a development tool, no more.
*/
if (!$IS_MYSQLND)
	die("skip Test has been written for the latest version of mysqlnd only");
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

Inspecting property 'client_info'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'client_version'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'driver_version'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'embedded'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'reconnect'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'report_mode'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256
Default property 'client_info'
Default property 'client_version'
Default property 'driver_version'
Default property 'embedded'
Default property 'reconnect'
Default property 'report_mode'
done!
