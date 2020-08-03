--TEST--
Type inference for $ary[$idx]->prop +=
--FILE--
<?php

function test() {
    $ary = [];
    $ary[0]->y += 2;
    var_dump(is_object($ary[0]));
}
try {
    test();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: Undefined array key 0 in %s on line %d
Attempt to assign property "y" on null
