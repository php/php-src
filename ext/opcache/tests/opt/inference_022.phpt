--TEST--
Type inference 022: FETCH_DIM_W
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function &foo(&$a, $n) {
    foreach (array(0) as $_) {
        return $a[$n];
    }
}
?>
DONE
--EXPECT--
DONE
