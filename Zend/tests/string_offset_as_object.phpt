--TEST--
Using string offset as object
--FILE--
<?php

$str = "x";
try {
    $str[0]->bar = "xyz";
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $str[0]->bar[1] = "bang";
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $str[0]->bar += 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $str[0]->bar = &$b;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    ++$str[0]->bar;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    --$str[0]->bar;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $str[0]->bar++;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $str[0]->bar--;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unset($str[0]->bar);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot use string offset as an object
Error: Cannot use string offset as an object
Error: Cannot use string offset as an object
Error: Cannot use string offset as an object
Error: Cannot use string offset as an object
Error: Cannot use string offset as an object
Error: Cannot use string offset as an object
Error: Cannot use string offset as an object
Error: Cannot use string offset as an object
