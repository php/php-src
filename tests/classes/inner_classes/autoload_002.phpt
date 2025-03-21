--TEST--
ensure private autoloading works
--FILE--
<?php
spl_autoload_register(static function ($class_name) {
    require_once(__DIR__ . '/' . $class_name . '.inc');
    echo 'autoload(' . $class_name . ")\n";
});

$line = new inner_classes:>Line();
var_dump($line);
?>
--EXPECTF--
autoload(inner_classes)

Fatal error: Cannot access private inner class 'inner_classes:>Line' in %s
