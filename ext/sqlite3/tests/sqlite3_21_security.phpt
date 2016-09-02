--TEST--
SQLite3 open_basedir checks
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--INI--
open_basedir=.
--FILE--
<?php
chdir(__DIR__);

$directory = dirname(__FILE__) . '/';
$file = uniqid() . '.db';

echo "Within test directory\n";
$db = new SQLite3($directory . $file);
var_dump($db);
var_dump($db->close());
unlink($directory . $file);

echo "Above test directory\n";
try {
	$db = new SQLite3('../bad' . $file);
} catch (Exception $e) {
	echo $e . "\n";
}

echo "Done\n";
?>
--EXPECTF--
Within test directory
object(SQLite3)#%d (0) {
}
bool(true)
Above test directory

Warning: SQLite3::__construct(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (.) in %ssqlite3_21_security.php on line %d
Exception: open_basedir prohibits opening %s in %ssqlite3_21_security.php:%d
Stack trace:
#0 %ssqlite3_21_security.php(%d): SQLite3->__construct('%s')
#1 {main}
Done
