--TEST--
Bug #35248 (sqlite_query does not return parse error message)
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php
	$db = sqlite_open(":memory:");
	$res = @sqlite_query($db, "asdfesdfa", SQLITE_NUM, $err);
	var_dump($err);
	$res = @sqlite_unbuffered_query($db, "asdfesdfa", SQLITE_NUM, $err);
	var_dump($err);
?>
--EXPECT--
string(30) "near "asdfesdfa": syntax error"
string(30) "near "asdfesdfa": syntax error"
