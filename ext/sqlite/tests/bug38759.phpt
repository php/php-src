--TEST--
Bug #38759 (sqlite2 empty query causes segfault)
--SKIPIF--
<?php 
if (!extension_loaded("pdo")) print "skip"; 
if (!extension_loaded("sqlite")) print "skip"; 
?>
--FILE--
<?php

$dbh = new PDO('sqlite2::memory:');
var_dump($dbh->query(" "));

echo "Done\n";
?>
--EXPECTF--	
bool(false)
Done
