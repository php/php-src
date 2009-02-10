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
	echo $e->getMessage() . "\n";
}
echo "Done\n";
unlink($unreadable);
?>
--EXPECTF--
Unable to open database: unable to open database file
Done
