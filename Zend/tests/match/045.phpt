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
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Undefined constant "x"
