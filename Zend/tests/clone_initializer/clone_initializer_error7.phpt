--TEST--
Test "clone with" with dynamic properties of an object throwing an exception in the destructor
--FILE--
<?php

#[AllowDynamicProperties]
class Foo
{
    public function __destruct() {
        throw new Exception("Error in destructor");
    }
}

function returnFoo() {
    return new Foo();
}

try {
    clone returnFoo() with ["bar" => new stdClass()];
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    clone returnFoo() with ["bar" => new stdClass()]; // The same as above but now using cache slots
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Error in destructor
Error in destructor
