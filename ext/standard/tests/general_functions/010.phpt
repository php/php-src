--TEST--
register_shutdown_function() & __call
--FILE--
<?php
class test {
    function _foo() {
        throw new Exception('test');
    }
    function __call($name=null, $args=null) {
        return test::_foo();
    }
}

try {
    var_dump(register_shutdown_function(array("test","__call")));
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
TypeError: register_shutdown_function(): Argument #1 ($callback) must be a valid callback, non-static method test::__call() cannot be called statically
Done
