--TEST--
ReflectionClass::getProperty() preserves the case of the class name in errors and autoloading
--FILE--
<?php
class C {
    public $p;
}

spl_autoload_register(function ($name) {
    echo "Autoloading $name\n";
});

$rc = new ReflectionClass('C');
try {
    $rc->getProperty("Some\\MixedCase::p");
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Autoloading Some\MixedCase
Class "Some\MixedCase" does not exist
