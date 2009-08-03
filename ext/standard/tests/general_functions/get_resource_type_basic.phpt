--TEST--
Test get_resource_type() function : basic functionality 
--FILE--
<?php
/* Prototype  : string get_resource_type  ( resource $handle  )
 * Description:  Returns the resource type 
 * Source code: Zend/zend_builtin_functions.c
 */		

echo "*** Testing get_resource_type() : basic functionality ***\n";

$res = fopen(__FILE__, "r");
var_dump(get_resource_type($res)); 

?>
===DONE===
--EXPECT--
*** Testing get_resource_type() : basic functionality ***
string(6) "stream"
===DONE===
