--TEST--
SQLite3::open error test
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
	die('skip windows only test');
}
require_once(__DIR__ . '/skipif.inc');
?>
--FILE--
<?php
$sysroot = exec('echo %systemroot%');
$icacls = "$sysroot\\System32\\icacls.exe";
$user = get_current_user();
$unreadable = __DIR__ . '/unreadable.db';

touch($unreadable);
$cmd = $icacls . ' ' . $unreadable . ' /inheritance:r /deny ' . $user . ':(F,M,R,RX,W)';
exec($cmd);

try {
	$db = new SQLite3($unreadable);
} catch (Exception $e) {
	echo $e . "\n";
}
echo "Done\n";

$cmd = $icacls . ' ' . $unreadable . ' /grant ' . $user . ':(F,M,R,RX,W)';
exec($cmd);
unlink($unreadable);
?>
--EXPECTF--
Exception: Unable to open database: %s in %ssqlite3_15_open_error-win.php:%d
Stack trace:
#0 %ssqlite3_15_open_error-win.php(%d): SQLite3->__construct('%s')
#1 {main}
Done
