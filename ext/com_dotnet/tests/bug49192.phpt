--TEST--
Bug #49192 (PHP crashes when GC invoked on COM object)
--SKIPIF--
<?php
if (!extension_loaded("com_dotnet")) print "skip COM/.Net support not present"; ?>
--FILE--
<?php

$dbConnection = new Com('ADODB.Connection');
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(0)
