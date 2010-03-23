--TEST--
Ensure extends does trigger autoload.
--FILE--
<?php
  function __autoload($name)
  {
      echo "In autoload: ";
      var_dump($name);
  }
  
  class C extends UndefBase
  {
  }
?>
--EXPECTF--
In autoload: string(9) "UndefBase"

Fatal error: Class 'UndefBase' not found in %s on line %d
