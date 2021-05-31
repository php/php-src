--TEST--
Bug #79247 (Garbage collecting variant objects segfaults)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
$keep = new variant(null);
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(0)
