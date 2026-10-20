--TEST--
Corrupted CFG due to unreachable free with match
--FILE--
<?php
function test() {
    var_dump(match(x){});
    match(y){
        3, 4 => 5,
    };
}
try {
    test();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Undefined constant "x"
