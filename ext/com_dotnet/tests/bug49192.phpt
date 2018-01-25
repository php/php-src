--TEST--
Bug #49192 (PHP crashes when GC invoked on COM object)
--SKIPIF--
<?php
if (!extension_loaded("com_dotnet")) print "skip COM/.Net support not present"; ?>
--XFAIL--
1
--FILE--
<?php

// this test fails to load ADO
//
// a change in windows longhorn x64(affecting vista, 7, 8, 2008, 2008r2) broke ADO.
//
// there is a fix available, but user has to install it.
// given that ADO was deprecated a long time ago in favor of newer APIs,
// I don't think its worth the trouble of making the user install the fix to
// get an accurate test run. its better to just not run the test or expect it to fail.
//
// see: http://support.microsoft.com/kb/2517589
// see: http://www.infoq.com/news/2011/10/ADO-Win7

$dbConnection = new Com('ADODB.Connection');
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(0)
