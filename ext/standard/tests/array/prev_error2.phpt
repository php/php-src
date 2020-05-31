--TEST--
prev - ensure warning is received when passing an indirect temporary.
--FILE--
<?php
/* Prototype  : mixed prev(array $array_arg)
 * Description: Move array argument's internal pointer to the previous element and return it
 * Source code: ext/standard/array.c
 */

/*
 * Pass temporary variables to prev() to test behaviour
 */

function f() {
    $array  = array(1,2);
    end($array);
    return $array;
}

echo "\n-- Passing an indirect temporary variable --\n";
try {
	var_dump(prev(f()));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
?>
--EXPECTF--
-- Passing an indirect temporary variable --
Exception: Cannot pass parameter 1 by reference
