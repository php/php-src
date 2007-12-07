--TEST--
Bug #43343 (Variable class name)
--FILE--
<?php
namespace Foo;
class Bar { }
$foo = 'bar';
var_dump(new namespace::$foo);
?>
--EXPECTF--
Fatal error: Cannot use 'namespace' as a class name in %sbug43343.php on line 5
