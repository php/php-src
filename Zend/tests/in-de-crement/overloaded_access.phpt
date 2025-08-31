--TEST--
Overloaded array access with pre increment/decrement
--FILE--
<?php
set_error_handler(function($severity, $m) {
	if (str_starts_with($m, 'Indirect modification of overloaded element')) { return; }
	throw new Exception($m, $severity);
});
class Foo implements ArrayAccess {
    function offsetGet($index): mixed {
        return range(1, 5);
    }
    function offsetSet($index, $newval): void {
    }
    function offsetExists($index): bool {
        return true;
    }
    function offsetUnset($index): void {
    }
}
$foo = new Foo;
try {
	$foo[0]++;
} catch (Throwable $ex) {
	echo $ex->getMessage() . "\n";
}
$foo = new Foo;
try {
	$foo[0]--;
} catch (Throwable $ex) {
	echo $ex->getMessage() . "\n";
}
?>
--EXPECT--
Cannot increment array
Cannot decrement array
