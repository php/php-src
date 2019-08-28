--TEST--
Ensure instanceof does not trigger autoload.
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "In autoload: ";
  var_dump($name);
});

$a = new StdClass;
var_dump($a instanceof UndefC);
?>
--EXPECT--
bool(false)
