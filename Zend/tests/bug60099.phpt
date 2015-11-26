--TEST--
Bug #60099 (__halt_compiler() works in braced namespaces)
--FILE--
<?php
namespace foo {
	__halt_compiler();

?>
--EXPECTF--
Parse error: syntax error, unexpected end of file in %s on line %d
