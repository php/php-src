--TEST--
GH-11781: Constant property assignment
--FILE--
<?php
const test = 42;
try {
    var_dump(false->x = 42);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(false->x->y = 42);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Attempt to assign property "x" on false
Attempt to modify property "x" on false
