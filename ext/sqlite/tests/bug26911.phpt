--TEST--
Bug #26911 (crash when fetching data from empty queries)
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php
	$db = sqlite_open(":memory:");
	$a = sqlite_query($db, "  ");
	echo "I am ok\n";
?>
--EXPECT--
I am ok
