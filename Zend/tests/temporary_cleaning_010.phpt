--TEST--
Live range & throw from finally
--FILE--
<?php
function test() {
    try {
        $a = [1, 2, 3];
        return $a + [];
    } finally {
        throw new Exception;
    }
}

try {
    test();
} catch (Exception $e) {
	echo "exception\n";
}
?>
--EXPECT--
exception
