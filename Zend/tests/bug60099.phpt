--TEST--
Bug #60099 (__halt_compiler() works in braced namespaces)
--FILE--
<?php
namespace foo {
	__halt_compiler();

?>
--EXPECTF--
Fatal error: __HALT_COMPILER() can only be used from the outermost scope in %s on line %d
