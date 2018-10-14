--TEST--
This test illustrates the impact of invoking destructors when refcount is decremented to 0 on foreach.
It will pass only if the 'contentious code' in PHPValue.decReferences() is enabled.
--FILE--
<?php

$a = array(1,2,3);
$container = array(&$a);

// From php.net:
//   "Unless the array is referenced, foreach operates on a copy of
//    the specified array and not the array itself."
// At this point, the array $a is referenced.

// The following line ensures $a is no longer references as a consequence
// of running the 'destructor' on $container.
$container = null;

// At this point the array $a is no longer referenced, so foreach should operate on a copy of the array.
// However, P8 does not invoke 'destructors' when refcount is decremented to 0.
// Consequently, $a thinks it is still referenced, and foreach will operate on the array itself.
// This provokes a difference in behaviour when changing the number of elements in the array while
// iterating over it.

$i=0;
foreach ($a as $v) {
	array_push($a, 'new');
	var_dump($v);

	if (++$i>10) {
		echo "Infinite loop detected\n";
		break;
	}
}

?>
--EXPECT--
int(1)
int(2)
int(3)
