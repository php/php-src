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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: test(): Return value must be of type int, string returned
