--TEST--
ZE2 Autoload and derived classes
--SKIPIF--
<?php
    if (class_exists('autoload_root', false)) die('skip Autoload test classes exist already');
?>
--FILE--
<?php

spl_autoload_register(function ($class_name) {
    require_once(__DIR__ . '/' . $class_name . '.inc');
    echo 'autoload(' . $class_name . ")\n";
});

var_dump(class_exists('autoload_derived'));

?>
--EXPECT--
autoload(autoload_root)
autoload(autoload_derived)
bool(true)
