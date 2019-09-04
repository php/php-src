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

?>
--EXPECT--
Cannot call abstract method Test::method()
