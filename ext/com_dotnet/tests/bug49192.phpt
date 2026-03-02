--TEST--
Bug #49192 (PHP crashes when GC invoked on COM object)
--EXTENSIONS--
com_dotnet
--FILE--
<?php

$dbConnection = new Com('ADODB.Connection');
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(0)
