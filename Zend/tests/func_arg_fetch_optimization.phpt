--TEST--
FETCH_DIM_FUNC_ARG that cannot be optimized to FETCH_DIM_R because it appends
--FILE--
<?php
function test($x) {
    test($x[][$y]);
}
try {
    test([]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot use [] for reading
