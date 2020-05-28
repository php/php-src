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

register_shutdown_function(array("test","__call"));

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: register_shutdown_function(): Argument #1 ($function) must be of type callable, array given in %s:%d
Stack trace:
#0 %s(%d): register_shutdown_function(Array)
#1 {main}
  thrown in %s on line %d
