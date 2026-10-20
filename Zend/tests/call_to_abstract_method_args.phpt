--TEST--
Check that arguments are freed when a call to an abstract method throws
--FILE--
<?php

abstract class Test {
    abstract static function method();
}

try {
    Test::method(new stdClass);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$ret = new stdClass;
try {
    $ret = Test::method(new stdClass);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot call abstract method Test::method()
Error: Cannot call abstract method Test::method()
