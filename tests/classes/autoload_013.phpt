--TEST--
Ensure the ReflectionClass constructor triggers autoload.
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "In autoload: ";
  var_dump($name);
});

try {
  new ReflectionClass("UndefC");
}
catch (ReflectionException $e) {
  echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
In autoload: string(6) "UndefC"
ReflectionException: Class "UndefC" does not exist
