--TEST--
080: bracketed namespaces and __HALT_COMPILER();
--FILE--
<?php
namespace foo {
echo "hi\n";
}
__HALT_COMPILER();
namespace unprocessed {
echo "should not echo\n";
}
?>
===DONE===
--EXPECT--
hi
