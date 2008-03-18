--TEST--
Interface of the class mysqli - Reflection
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
	$class = new ReflectionClass('mysqli');
	inspectClass($class);
	print "done!\n";
?>
--EXPECTF--
Inspecting class 'mysqli'
isInternal: yes
isUserDefined: no
isInstantiable: yes
isInterface: no
isAbstract: no
isFinal: no
isIteratable: no
Modifiers: '0'
Parent Class: ''
Extension: 'mysqli'

Inspecting method 'mysqli'
isFinal: no
isAbstract: no
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isConstructor: yes
isDestructor: no
isInternal: yes
isUserDefined: no
returnsReference: no
Modifiers: 8448
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'autocommit'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'change_user'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'character_set_name'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'client_encoding'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'close'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'commit'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'connect'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'debug'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'dump_debug_info'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'escape_string'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'get_charset'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'get_client_info'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'get_connection_stats'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'get_server_info'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'get_warnings'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'init'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'kill'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'more_results'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'multi_query'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'mysqli'
isFinal: no
isAbstract: no
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isConstructor: yes
isDestructor: no
isInternal: yes
isUserDefined: no
returnsReference: no
Modifiers: 8448
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'next_result'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'options'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'ping'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'prepare'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'query'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'real_connect'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'real_escape_string'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'real_query'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'rollback'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'select_db'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'set_charset'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'set_opt'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'stat'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'stmt_init'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'store_result'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'thread_safe'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'use_result'
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
Modifiers: 256
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting property 'affected_rows'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

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

Inspecting property 'connect_errno'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'connect_error'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'errno'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'error'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'field_count'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'host_info'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'info'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'insert_id'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'protocol_version'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'server_info'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'server_version'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'sqlstate'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'thread_id'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256

Inspecting property 'warning_count'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 256
Default property 'affected_rows'
Default property 'client_info'
Default property 'client_version'
Default property 'connect_errno'
Default property 'connect_error'
Default property 'errno'
Default property 'error'
Default property 'field_count'
Default property 'host_info'
Default property 'info'
Default property 'insert_id'
Default property 'protocol_version'
Default property 'server_info'
Default property 'server_version'
Default property 'sqlstate'
Default property 'thread_id'
Default property 'warning_count'
done!

