--TEST--
Resource offset behaviour
--FILE--
<?php

require dirname(__DIR__) . DIRECTORY_SEPARATOR . 'container_var.inc';
require dirname(__DIR__, 2) . DIRECTORY_SEPARATOR . 'test_const_resource_offsets.inc';

?>
--EXPECTF--
Read before write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line %d

Warning: Undefined array key 3 in %s on line %d
NULL
Write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line %d
Read:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line %d
string(1) "v"
Read-Write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line %d
isset():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line %d
bool(true)
empty():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line %d
bool(false)
Coalesce():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line %d
string(7) "vappend"
