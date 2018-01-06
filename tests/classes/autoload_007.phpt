--TEST--
Ensure instanceof does not trigger autoload.
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "In autoload: ";
  var_dump($name);
});

$a = new stdClass;
var_dump($a instanceof UndefC);
?>
--EXPECTF--
bool(false)
