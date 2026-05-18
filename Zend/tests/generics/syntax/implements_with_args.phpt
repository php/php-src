--TEST--
Generic syntax: implements with type arguments
--FILE--
<?php
interface Iface<T> {}
class C<U> implements Iface<U> {}
$r = new ReflectionClass('C');
foreach ($r->getInterfaceNames() as $n) echo $n, "\n";
?>
--EXPECT--
Iface
