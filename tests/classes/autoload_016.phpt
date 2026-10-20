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
  echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
In autoload: string(6) "UndefC"
ReflectionException: Class "UndefC" does not exist
