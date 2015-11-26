--TEST--
079: nested namespaces
--FILE--
<?php
namespace foo {
    namespace oops {
    }
}
?>
===DONE===
--EXPECTF--
Fatal error: Namespace declarations cannot be nested in %s on line %d
