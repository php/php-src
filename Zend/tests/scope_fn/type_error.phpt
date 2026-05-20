--TEST--
Type error in scope function parameter is catchable
--FILE--
<?php
function test() {
    $fn = fn(int $a) { return $a; };
    try {
        $fn("not an int");
    } catch (TypeError $e) {
        echo "Caught TypeError\n";
    }
    echo "ok\n";
}
test();
?>
--EXPECT--
Caught TypeError
ok
