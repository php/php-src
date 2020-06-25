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
Parse error: %s error%sexpecting%s"\"%sin %sbug43343.php on line 5
