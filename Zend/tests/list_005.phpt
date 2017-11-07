--TEST--
Testing list() with several variables
--FILE--
<?php

$str = "foo";

list($a, $b, $c) = $str;

var_dump($a, $b, $c);

print "----\n";

$int = 1;

list($a, $b, $c) = $int;

var_dump($a, $b, $c);

print "----\n";

$obj = new stdClass;

list($a, $b, $c) = $obj;

var_dump($a, $b, $c);

print "----\n";

$arr = array(1, 2, 3);

list($a, $b, $c) = $arr;

var_dump($a, $b, $c);

?>
--EXPECTF--
Warning: Variable of type string does not accept array offsets in %s on line %d

Warning: Variable of type string does not accept array offsets in %s on line %d

Warning: Variable of type string does not accept array offsets in %s on line %d
NULL
NULL
NULL
----

Warning: Variable of type int does not accept array offsets in %s on line %d

Warning: Variable of type int does not accept array offsets in %s on line %d

Warning: Variable of type int does not accept array offsets in %s on line %d
NULL
NULL
NULL
----

Fatal error: Uncaught Error: Cannot use object of type stdClass as array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
