--TEST--
Error paths in scope function don't leak VM stack frames
--FILE--
<?php
function make() {
    return fn() { return 1; };
}
function test_recursion() {
    $ref = null;
    $fn = fn() {
        $ref();
    };
    $ref = $fn;
    try {
        $fn();
    } catch (Error) {}
}
for ($i = 0; $i < 1000; $i++) {
    try { make(); } catch (Error) {}
}
echo "lifetime: ok\n";
for ($i = 0; $i < 1000; $i++) { test_recursion(); }
echo "recursion: ok\n";
?>
--EXPECT--
lifetime: ok
recursion: ok
