--TEST--
Bug #60099 (__halt_compiler() works in braced namespaces)
--FILE--
<?php
namespace foo {
    __halt_compiler();

?>
--EXPECTF--
Parse error: Unclosed '{' on line 2 in %s on line %d
