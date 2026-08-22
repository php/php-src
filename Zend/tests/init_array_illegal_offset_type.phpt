--TEST--
INIT_ARRAY with illegal offset type
--FILE--
<?php
function test() {
    return [new stdClass => null];
}
try {
    test();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: Cannot access offset of type stdClass on array
