--TEST--
register_shutdown_function() and long-term fci storage
--FILE--
<?php
class Test {
    function register() {
        register_shutdown_function('Test::method');
    }
    function method() {
        var_dump($this);
    }
}
(new Test)->register();
?>
--EXPECT--
object(Test)#1 (0) {
}
