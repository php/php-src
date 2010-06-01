--TEST--
SQLite3::open error test
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
$unreadable = dirname(__FILE__) . '/unreadable.db';
touch($unreadable);
chmod($unreadable,  0200);
try {
	$db = new SQLite3($unreadable);
} catch (Exception $e) {
	echo $e . "\n";
}
echo "Done\n";
unlink($unreadable);
?>
--EXPECTF--
exception 'Exception' with message 'Unable to open database: %s' in %s/sqlite3_15_open_error.php:%d
Stack trace:
#0 %s/sqlite3_15_open_error.php(%d): SQLite3->__construct('%s')
#1 {main}
Done
