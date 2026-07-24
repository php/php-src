--TEST--
Pipe assign operator AST printing
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

try {
    assert(false && $x |>= foo(...));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && $x |>= foo(...) |> bar(...));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && $x |>= (fn($v) => $v * 2));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && $arr[0] |>= foo(...));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && $obj->prop |>= foo(...));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && $x |>= ($callback ?? fallback(...)));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && $x |>= ($cond ? foo(...) : bar(...)));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && $x |>= $y |>= foo(...));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && $z = $x |>= foo(...));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && ($x |>= foo(...)) |> bar(...));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
assert(false && ($x |>= foo(...)))
assert(false && ($x |>= foo(...) |> bar(...)))
assert(false && ($x |>= (fn($v) => $v * 2)))
assert(false && ($arr[0] |>= foo(...)))
assert(false && ($obj->prop |>= foo(...)))
assert(false && ($x |>= $callback ?? fallback(...)))
assert(false && ($x |>= $cond ? foo(...) : bar(...)))
assert(false && ($x |>= $y |>= foo(...)))
assert(false && ($z = $x |>= foo(...)))
assert(false && ($x |>= foo(...)) |> bar(...))
