--TEST--
Using string offset as object
--FILE--
<?php

$str = "x";
try {
    $str[0]->bar = "xyz";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $str[0]->bar[1] = "bang";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $str[0]->bar += 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $str[0]->bar = &$b;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    ++$str[0]->bar;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    --$str[0]->bar;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $str[0]->bar++;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $str[0]->bar--;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($str[0]->bar);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Attempt to assign property "bar" on string
Attempt to modify property "bar" on string
Attempt to assign property "bar" on string
Attempt to modify property "bar" on string
Attempt to increment/decrement property "bar" on string
Attempt to increment/decrement property "bar" on string
Attempt to increment/decrement property "bar" on string
Attempt to increment/decrement property "bar" on string
Cannot use string offset as an object
