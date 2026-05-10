--TEST--
Traits: ReflectionClass::isGeneric on trait
--FILE--
<?php
trait T<X> {}
trait U {}
echo (new ReflectionClass('T'))->isGeneric() ? "T:gen\n" : "T:not\n";
echo (new ReflectionClass('U'))->isGeneric() ? "U:gen\n" : "U:not\n";
?>
--EXPECT--
T:gen
U:not
