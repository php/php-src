--TEST--
autoloading inner classes
--FILE--
<?php

spl_autoload_register(function ($class_name) {
    require_once(__DIR__ . '/' . $class_name . '.inc');
    echo 'autoload(' . $class_name . ")\n";
});

var_dump(class_exists(inner_class_autoload::Point::class));
?>
--EXPECT--
autoload(inner_class_autoload)
bool(true)
