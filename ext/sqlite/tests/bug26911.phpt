--TEST--
Bug #26911 (crash when fetching data from empty queries)
--FILE--
<?php
	$db = sqlite_open(":memory:");
	$a = sqlite_query($db, "  ");
	echo "I am ok\n";
?>
--EXPECT--
I am ok
