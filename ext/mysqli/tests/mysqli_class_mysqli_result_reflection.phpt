--TEST--
Interface of the class mysqli_result - Reflection
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
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
    $class = new ReflectionClass('mysqli_result');
    inspectClass($class);
    print "done!";
?>
--EXPECT--
Inspecting class 'mysqli_result'
isInternal: yes
isUserDefined: no
isInstantiable: yes
isInterface: no
isAbstract: no
isFinal: no
isIteratable: yes
Modifiers: '0'
Parent Class: ''
Extension: 'mysqli'

Inspecting method '__construct'
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
Modifiers: 1
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method '__construct'
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
Modifiers: 1
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
Modifiers: 1
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'data_seek'
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
Modifiers: 1
Number of Parameters: 1
Number of Required Parameters: 1

Inspecting parameter 'offset' of method 'data_seek'
isArray: no
allowsNull: no
isPassedByReference: no
isOptional: no
isDefaultValueAvailable: no

Inspecting method 'fetch_all'
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
Modifiers: 1
Number of Parameters: 1
Number of Required Parameters: 0

Inspecting parameter 'result_type' of method 'fetch_all'
isArray: no
allowsNull: no
isPassedByReference: no
isOptional: yes
isDefaultValueAvailable: no

Inspecting method 'fetch_array'
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
Modifiers: 1
Number of Parameters: 1
Number of Required Parameters: 0

Inspecting parameter 'result_type' of method 'fetch_array'
isArray: no
allowsNull: no
isPassedByReference: no
isOptional: yes
isDefaultValueAvailable: no

Inspecting method 'fetch_assoc'
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
Modifiers: 1
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'fetch_field'
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
Modifiers: 1
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'fetch_field_direct'
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
Modifiers: 1
Number of Parameters: 1
Number of Required Parameters: 1

Inspecting parameter 'field_nr' of method 'fetch_field_direct'
isArray: no
allowsNull: no
isPassedByReference: no
isOptional: no
isDefaultValueAvailable: no

Inspecting method 'fetch_fields'
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
Modifiers: 1
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'fetch_object'
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
Modifiers: 1
Number of Parameters: 2
Number of Required Parameters: 0

Inspecting parameter 'class_name' of method 'fetch_object'
isArray: no
allowsNull: no
isPassedByReference: no
isOptional: yes
isDefaultValueAvailable: no

Inspecting parameter 'params' of method 'fetch_object'
isArray: yes
allowsNull: no
isPassedByReference: no
isOptional: yes
isDefaultValueAvailable: no

Inspecting method 'fetch_row'
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
Modifiers: 1
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'field_seek'
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
Modifiers: 1
Number of Parameters: 1
Number of Required Parameters: 1

Inspecting parameter 'field_nr' of method 'field_seek'
isArray: no
allowsNull: no
isPassedByReference: no
isOptional: no
isDefaultValueAvailable: no

Inspecting method 'free'
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
Modifiers: 1
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting method 'free_result'
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
Modifiers: 1
Number of Parameters: 0
Number of Required Parameters: 0

Inspecting property 'current_field'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 1

Inspecting property 'field_count'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 1

Inspecting property 'lengths'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 1

Inspecting property 'num_rows'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 1

Inspecting property 'type'
isPublic: yes
isPrivate: no
isProtected: no
isStatic: no
isDefault: yes
Modifiers: 1
Default property 'current_field'
Default property 'field_count'
Default property 'lengths'
Default property 'num_rows'
Default property 'type'
done!
