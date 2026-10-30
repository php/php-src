--TEST--
GH-18736: Circumvented type check with return by ref + finally
--FILE--
<?php

function &test(): int {
    $x = 0;
    try {
        return $x;
    } finally {
        $x = 'test';
    }
}

try {
    $x = &test();
    var_dump($x);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
test(): Return value must be of type int, string returned
