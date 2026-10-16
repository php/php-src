--TEST--
Session files handler must not truncate session file when write fails
--EXTENSIONS--
session
posix
pcntl
--INI--
error_reporting=E_ALL
display_errors=1
session.use_strict_mode=0
session.save_handler=files
session.serialize_handler=php
--FILE--
<?php
$dir = __DIR__ . '/session_write_failure_keeps_data';
mkdir($dir);
ini_set('session.save_path', $dir);

$sid = 'write-failure';
$file = $dir . '/sess_' . $sid;

ob_start();

session_id($sid);
session_start();
$_SESSION['data'] = 'original data';
session_write_close();

$before = file_get_contents($file);
echo "Original contents: ";
var_dump($before);

// A zero file size limit makes the next write fail before any byte lands;
// SIGXFSZ has to be ignored or the process is killed instead.
pcntl_signal(SIGXFSZ, SIG_IGN);
echo "Set file size limit to zero: ";
var_dump(posix_setrlimit(POSIX_RLIMIT_FSIZE, 0, POSIX_RLIMIT_INFINITY));

session_start();
$_SESSION['data'] = 'new';
@session_write_close();

echo "Contents unchanged after failed write: ";
var_dump(file_get_contents($file) === $before);

ob_end_flush();
?>
--CLEAN--
<?php
$dir = __DIR__ . '/session_write_failure_keeps_data';
foreach (glob($dir . '/sess_*') as $f) {
    unlink($f);
}
rmdir($dir);
?>
--EXPECT--
Original contents: string(26) "data|s:13:"original data";"
Set file size limit to zero: bool(true)
Contents unchanged after failed write: bool(true)
