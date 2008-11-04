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
Parse error: syntax error, unexpected T_PAAMAYIM_NEKUDOTAYIM, expecting T_NS_SEPARATOR in %sbug43343.php on line 5
