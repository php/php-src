--TEST--
Check that arguments are freed when a call to an abstract method throws
--FILE--
<?php

abstract class Test {
    abstract static function method();
}

try {
    Test::method(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$ret = new stdClass;
try {
    $ret = Test::method(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Abstract method Test::method() cannot be called
Abstract method Test::method() cannot be called
