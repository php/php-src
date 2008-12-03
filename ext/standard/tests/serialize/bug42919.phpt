--TEST--
Bug #42919 (Unserializing of namespaced class object fails)
--FILE--
<?php
namespace Foo;
class Bar {
}
echo serialize(new Bar) . "\n";
$x = unserialize(serialize(new Bar));
echo get_class($x) . "\n";
?>
--EXPECT--
O:8:"Foo::Bar":0:{}
Foo::Bar
