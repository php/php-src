--TEST--
match expression always errors
--FILE--
<?php
function get_value() {
    return 0;
}
function test() {
    match(get_value()) {
        false => $a,
        true => $b,
    };
}
try {
    test();
} catch (UnhandledMatchError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Unhandled match case 0
