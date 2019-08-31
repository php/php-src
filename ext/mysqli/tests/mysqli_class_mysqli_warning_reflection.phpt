--TEST--
Interface of the class mysqli_stmt - Reflection
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('connect.inc');

/*
Let's not deal with cross-version issues in the EXPECTF section.
Most of the things which we test are covered by mysqli_class_*_interface.phpt.
Those tests go into the details and are aimed to be a development tool, no more.
*/
if (!$IS_MYSQLND)
	die("skip Test has been written for mysqlnd only");
?>
--FILE--
<?php
	require_once('reflection_tools.inc');
	$class = new ReflectionClass('mysqli_warning');
	inspectClass($class);
	print "done!\n";
?>
--EXPECTF--
Inspecting class 'mysqli_warning'
isInternal: yes
isUserDefined: no
isInstantiable: no
isInterface: no
isAbstract: no
isFinal: yes
isIteratable: no
Modifiers: '%d'
Parent Class: ''
Extension: 'mysqli'

Inspecting method '__construct'
isFinal: no
isAbstract: no
isPublic: no
isPrivate: no
isProtected: yes
isStatic: no
isConstructor: yes
isDestructor: no
isInternal: yes
isUserDefined: no
returnsReference: no
Modifiers: %d
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method '__construct'
isFinal: no
isAbstract: no
isPublic: no
isPrivate: no
isProtected: yes
isStatic: no
isConstructor: yes
isDestructor: no
isInternal: yes
isUserDefined: no
returnsReference: no
Modifiers: %d
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'next'
isFinal: no
isAbstract: no
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isConstructor: no
isDestructor: no
isInternal: yes
isUserDefined: no
returnsReference: no
Modifiers: %d
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting property 'errno'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 1

Inspecting property 'message'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 1

Inspecting property 'sqlstate'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 1
Default property 'errno'
Default property 'message'
Default property 'sqlstate'
done!
