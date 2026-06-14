--TEST--
Bug #70187 (Notice: unserialize(): Unexpected end of serialized data)
--FILE--
<?php
$a = 1;
unset($a);
unserialize(serialize($GLOBALS));
echo "ok\n";
?>
--EXPECT--
ok
