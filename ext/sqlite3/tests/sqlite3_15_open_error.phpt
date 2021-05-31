--TEST--
SQLite3::open error test
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
    die('skip non windows test');
}
if (!function_exists('posix_geteuid')) {
    die('SKIP posix_geteuid() not defined so cannot check if run as root');
}
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
Exception: Unable to open database: %s in %ssqlite3_15_open_error.php:%d
Stack trace:
#0 %ssqlite3_15_open_error.php(%d): SQLite3->__construct('%s')
#1 {main}
Done
