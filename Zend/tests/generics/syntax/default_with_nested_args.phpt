--TEST--
Generic syntax: >>= splitting in default position
--FILE--
<?php
class MapT {}
class C<T = MapT<int, string>> {}
$p = (new ReflectionClass('C'))->getGenericParameters()[0];
echo $p, "\n";
?>
--EXPECT--
T = MapT
