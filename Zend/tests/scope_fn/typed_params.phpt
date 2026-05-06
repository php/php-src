--TEST--
Scope function with typed parameters
--FILE--
<?php
function test() {
    $fn = fn(int $a, string $b): string { return $b . $a; };
    var_dump($fn(42, "val="));
}
test();
?>
--EXPECT--
string(6) "val=42"
