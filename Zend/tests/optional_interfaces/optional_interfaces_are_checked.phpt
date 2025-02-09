--TEST--
Optional interfaces are checked
--FILE--
<?php

interface Iface
{
    public function method();
}

class DecentClass implements ?Iface
{
    public function method() {}
}

$anInstance = new DecentClass;
if ($anInstance instanceof Iface)
    echo "Existing interfaces can be implemented.";

class BadClass implements ?Iface {}

?>
--EXPECTF--
Existing interfaces can be implemented.
Fatal error: Class BadClass contains 1 abstract method and must therefore be declared abstract or implement the remaining method (Iface::method) in %soptional_interfaces_are_checked.php on line %d
