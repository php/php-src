--TEST--
Bug #54804 (__halt_compiler and imported namespaces)
--FILE--
<?php
namespace a;
require __DIR__ . '/bug54804.inc';
echo 'DONE';
__halt_compiler();
?>
--EXPECT--
DONE
