--TEST--
060: multiple namespaces per file
--FILE--
<?php
namespace Foo;
use Bar\A as B;
class A {}
$a = new B;
$b = new A;
echo get_class($a)."\n";
echo get_class($b)."\n";
namespace Bar;
use Foo\A as B;
$a = new B;
$b = new A;
echo get_class($a)."\n";
echo get_class($b)."\n";
class A {}
?>
--EXPECT--
Bar\A
Foo\A
Foo\A
Bar\A
