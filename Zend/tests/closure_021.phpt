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
} catch (Exception $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(5) "test!"
