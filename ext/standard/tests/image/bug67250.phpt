--TEST--
Bug #67250 (iptcparse out-of-bounds read)
--FILE--
<?php
var_dump(iptcparse("\x1C\x02_\x80___"));
?>
--EXPECT--
bool(false)
