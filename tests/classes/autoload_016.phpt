--TEST--
Ensure ReflectionClass::getProperty() triggers autoload
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "In autoload: ";
  var_dump($name);
});

$rc = new ReflectionClass("stdClass");

try {
$rc->getProperty("UndefC::p");
} catch (ReflectionException $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
In autoload: string(6) "undefc"
Class "undefc" does not exist
