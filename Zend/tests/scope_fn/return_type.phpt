--TEST--
Scope function with return type declaration
--FILE--
<?php
function test() {
    $fn = fn(): int { return 42; };
    var_dump($fn());

    $fn2 = fn(): string { return "hello"; };
    var_dump($fn2());
}
test();
?>
--EXPECT--
int(42)
string(5) "hello"
