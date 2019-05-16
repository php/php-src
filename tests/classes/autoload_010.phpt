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

Fatal error: Interface 'UndefI' not found in %s on line %d
