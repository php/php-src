--TEST--
Optional interfaces are autoloaded
--FILE--
<?php

spl_autoload_register(function ($class) {
    echo "Autoloading: $class\n";

    if ($class === 'ExistingInterface')
        eval("interface ExistingInterface {}");
});

class TestClass implements ?ExistingInterface, ?NonExistantInterface {}

$c = new TestClass;
echo implode(',', class_implements($c))."\n";
echo implode(',', class_implements('TestClass'))."\n";

?>
--EXPECT--
Autoloading: ExistingInterface
Autoloading: NonExistantInterface
ExistingInterface
ExistingInterface
