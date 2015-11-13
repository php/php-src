--TEST--
Live range & throw from finally
--XFAIL--
See Bug #62210 and attempt to fix it in "tmp_livelibess" branch
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
