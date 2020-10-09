--TEST--
Ensure object comparison property order remains consistent
--FILE--
<?php

// PHP4-5.3 object semantics had child properties added to an
// object HashTable first, then parent, then grandparent, etc...
// As of PHP 5.4 we use a packed C array to hold properties
// which may or may not share the same ordering.
// In the code snippet below, the print_r() has the side-effect
// of materializing the properties shadow HashTable which
// if used for comparison, results in the behavior consistent
// with pre PHP-5.4.
// This test ensures that the first comparison yields the same
// result without shadow table materialization.

class A { public $a; }
class B extends A { public $b; }
$a = new B(); $a->a = 0; $a->b = 1;
$b = new B(); $b->a = 1; $b->b = 0;

var_dump($a < $b);
print_r($a, true);
var_dump($a < $b);
?>
--EXPECT--
bool(false)
bool(false)
