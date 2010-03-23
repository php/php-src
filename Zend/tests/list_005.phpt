--TEST--
Testing list() with several variables
--FILE--
<?php

$a = "foo";

list($a, $b, $c) = $a;

var_dump($a, $b, $c);

print "----\n";

$a = 1;

list($a, $b, $c) = $a;

var_dump($a, $b, $c);

print "----\n";

$a = new stdClass;

list($a, $b, $c) = $a;

var_dump($a, $b, $c);

print "----\n";

$a = array(1, 2, 3);

list($a, $b, $c) = $a;

var_dump($a, $b, $c);

?>
--EXPECTF--
string(1) "f"
string(1) "o"
string(1) "o"
----
NULL
NULL
NULL
----

Fatal error: Cannot use object of type stdClass as array in %s on line %d
