--TEST--
SQLite3::open error test
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
	die('skip non windows test');
}
require_once(__DIR__ . '/skipif.inc');
if (posix_geteuid() == 0) {
    die('SKIP Cannot run test as root.');
}
?>
--FILE--
<?php
$unreadable = __DIR__ . '/unreadable.db';
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
exception 'Exception' with message 'Unable to open database: %s' in %ssqlite3_15_open_error.php:%d
Stack trace:
#0 %ssqlite3_15_open_error.php(%d): SQLite3->__construct('%s')
#1 {main}
Done
