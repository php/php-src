--TEST--
GH-11821 (exit() while autoloading a trait)
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
    use B;
}
echo "not reached\n";
?>
--EXPECT--
autoload B
shutdown
