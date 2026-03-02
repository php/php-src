--TEST--
Bug #36568 (memory_limit has no effect)
--INI--
memory_limit=16M
--FILE--
<?php
ini_set("memory_limit", "32M");
echo ini_get("memory_limit");
?>
--EXPECT--
32M
