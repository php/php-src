--TEST--
Ensure implements does trigger autoload.
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "In autoload: ";
  var_dump($name);
});

class C implements UndefI
{
}
?>
--EXPECTF--
In autoload: string(6) "UndefI"

Fatal error: Uncaught Error: Interface "UndefI" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
