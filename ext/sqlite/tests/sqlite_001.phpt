--TEST--
sqlite: sqlite_open/close
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
require_once('blankdb.inc');
echo "$db\n";
sqlite_close($db);
echo "Done\n";
?>
--EXPECTF--
Resource id #%d
Done
