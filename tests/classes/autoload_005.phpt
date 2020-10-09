--TEST--
ZE2 Autoload from destructor
--SKIPIF--
<?php
    if (class_exists('autoload_root', false)) die('skip Autoload test classes exist already');
?>
--FILE--
<?php

spl_autoload_register(function ($class_name) {
    var_dump(class_exists($class_name, false));
    require_once(__DIR__ . '/' . $class_name . '.inc');
    echo 'autoload(' . $class_name . ")\n";
});

var_dump(class_exists('autoload_derived', false));
var_dump(class_exists('autoload_derived', false));

class Test
{
    function __destruct() {
        echo __METHOD__ . "\n";
        $o = new autoload_derived;
        var_dump($o);
    }
}

$o = new Test;
unset($o);

?>
--EXPECTF--
bool(false)
bool(false)
Test::__destruct
bool(false)
bool(false)
autoload(autoload_root)
autoload(autoload_derived)
object(autoload_derived)#%d (0) {
}
