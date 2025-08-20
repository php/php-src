--TEST--
Some string offset errors
--FILE--
<?php

function &test() : string {
    $str = "foo";
    return $str[0];
}

function &gen() {
    $str = "foo";
    yield $str[0];
}

try {
    test();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    $str = "foo";
    $str[0] =& $str[1];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    foreach (gen() as $v) {}
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot create references to/from string offsets
Cannot create references to/from string offsets
Cannot create references to/from string offsets
