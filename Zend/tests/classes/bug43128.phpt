--TEST--
Bug #43128 (Very long class name causes segfault)
--INI--
memory_limit=128000000
--FILE--
<?php

$a = str_repeat("a", 10 * 1024 * 1024);

eval("class $a {}");

# call_user_func($a); // Warning
# $a->$a();           // Fatal error

if ($a instanceof $a); // Segmentation fault
new $a;                // Segmentation fault
echo "ok\n";
?>
--EXPECT--
ok
