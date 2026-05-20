--TEST--
Scope function that doesn't escape is fine
--FILE--
<?php
function test() {
    $fn = fn() { return 42; };
    var_dump($fn());
}
test();
echo "ok\n";
?>
--EXPECT--
int(42)
ok
