--TEST--
Bug #48679 (sqlite2 count on unbuffered query causes segfault)
--SKIPIF--
<?php 
if (!extension_loaded("sqlite")) print "skip"; 
?>
--FILE--
<?php

try {
	$x = new sqliteunbuffered;
	count($x);
} catch (SQLiteException $e) {
	var_dump($e->getMessage());
}
echo "Done\n";
?>
--EXPECT--	
string(41) "Row count is not available for this query"
Done
