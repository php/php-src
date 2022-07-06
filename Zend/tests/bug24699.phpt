--TEST--
Bug #24699 (Memory Leak with per-class constants)
--FILE--
<?php
class TEST { const FOO = SEEK_CUR; };
class TEST2 { const FOO = 1; };
class TEST3 { const FOO = PHP_VERSION; };
print TEST::FOO."\n";
?>
--EXPECT--
1
