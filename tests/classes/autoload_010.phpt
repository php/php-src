--TEST--
Ensure implements does trigger autoload.
--FILE--
<?php
  function __autoload($name)
  {
      echo "In autoload: ";
      var_dump($name);
  }
  
  class C implements UndefI
  {
  }
?>
--EXPECTF--
In autoload: string(6) "UndefI"

Fatal error: Interface 'UndefI' not found in %s on line %d
