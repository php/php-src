--TEST--
Ensure __autoload() recursion is guarded for multiple lookups of the same class, and that differently-cased names are distinct classes.
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "autoload $name\n";
  class_exists("undefinedCLASS");
});

class_exists("unDefinedClass");
?>
--EXPECT--
autoload unDefinedClass
autoload undefinedCLASS
