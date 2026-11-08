--TEST--
GH-11821 (exit() during delayed variance autoload)
--FILE--
<?php
register_shutdown_function(function () {
    echo "shutdown\n";
});
spl_autoload_register(function ($class) {
    echo "autoload $class\n";
    exit;
});
class A {
    function method(): X {}
}
class B extends A {
    function method(): Y {}
}
echo "not reached\n";
?>
--EXPECT--
autoload Y
shutdown
