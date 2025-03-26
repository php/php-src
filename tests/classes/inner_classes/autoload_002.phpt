--TEST--
ensure private autoloading works
--FILE--
<?php
spl_autoload_register(static function ($class_name) {
    require_once(__DIR__ . '/' . explode('\\', $class_name)[0] . '.inc');
    echo 'autoload(' . $class_name . ")\n";
});

$line = new inner_classes\Line();
var_dump($line);
?>
--EXPECTF--
autoload(inner_classes\Line)

Fatal error: Uncaught Error: Cannot instantiate class inner_classes\Line from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
