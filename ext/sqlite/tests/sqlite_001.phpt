--TEST--
SQLITE: sqlite_open/close
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
$db = sqlite_open("sqlite.db");
echo "$db\n";
sqlite_close($db);
echo "Done\n";
?>
--EXPECTF--
Resource id #%d
Done
