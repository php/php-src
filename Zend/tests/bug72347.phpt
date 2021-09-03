--TEST--
Bug #72347 (VERIFY_RETURN type casts visible in finally)
--FILE--
<?php
function test() : int {
    $d = 1.5;
    try {
        return $d;
    } finally {
        var_dump($d);
    }
}
var_dump(test());
?>
--EXPECTF--
Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
float(1.5)
int(1)
