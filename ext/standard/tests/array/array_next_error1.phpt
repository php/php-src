--TEST--
next - ensure warning is received when passing an indirect temporary.
--FILE--
<?php
function f() {
    return array(1, 2);
}
try {
	var_dump(next(f()));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Exception: Cannot pass parameter 1 by reference
