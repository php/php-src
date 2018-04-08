--TEST--
Ensure extends does trigger autoload.
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "In autoload: ";
  var_dump($name);
});

class C extends UndefBase
{
}
?>
--EXPECTF--
In autoload: string(9) "UndefBase"

Fatal error: Uncaught Error: Class 'UndefBase' not found in %s:%d
Stack trace:
#0 {main}
  thrown in %sautoload_011.php on line %d
