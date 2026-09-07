--TEST--
Closure 021: Throwing exception inside lambda
--FILE--
<?php

$foo = function() {
    try {
        throw new Exception('test!');
    } catch(Exception $e) {
        throw $e;
    }
};

try {
    $foo();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: test!
