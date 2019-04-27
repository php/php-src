--TEST--
Ensure catch blocks for unknown exception types do not trigger autoload.
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "In autoload: ";
  var_dump($name);
});

function f()
{
  throw new Exception();
}
try {
  f();
}
catch (UndefC $u) {
  echo "In UndefClass catch block.\n";
}
catch (Exception $e) {
  echo "In Exception catch block. Autoload should not have been triggered.\n";
}
?>
--EXPECT--
In Exception catch block. Autoload should not have been triggered.
