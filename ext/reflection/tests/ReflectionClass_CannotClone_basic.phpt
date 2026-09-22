--TEST--
Reflection class cannot be cloned
--CREDITS--
Stefan Koopmanschap <stefan@phpgg.nl>
TestFest PHP|Tek
--FILE--
<?php
$rc = new ReflectionClass("stdClass");
try {
  $rc2 = clone($rc);
} catch (Throwable $e) {
  echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: Trying to clone an uncloneable object of class ReflectionClass
