--TEST--
Bug #33989 (extract($GLOBALS,EXTR_REFS) crashes PHP)
--FILE--
<?php
$a="a";
extract($GLOBALS, EXTR_REFS);
echo "ok\n";
?>
--EXPECT--
ok
