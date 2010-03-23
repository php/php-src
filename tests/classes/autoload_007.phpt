--TEST--
Ensure instanceof does not trigger autoload.
--FILE--
<?php
  function __autoload($name)
  {
      echo "In autoload: ";
      var_dump($name);
  }
  
  $a = new stdClass;
  var_dump($a instanceof UndefC);
?>
--EXPECTF--
bool(false)
