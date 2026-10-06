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
--FILE--
<?php
$dir = __DIR__ . '/session_write_failure_keeps_data';
mkdir($dir);
ini_set('session.save_path', $dir);

$sid = 'apher5sessid12345678901234567890';
$file = $dir . '/sess_' . $sid;

ob_start();

session_id($sid);
session_start();
$_SESSION['data'] = str_repeat('A', 8192);
session_write_close();

clearstatcache(true);
$before = file_get_contents($file);
var_dump(strlen($before));

// A zero file size limit makes the next write fail before any byte lands;
// SIGXFSZ has to be ignored or the process is killed instead.
pcntl_signal(SIGXFSZ, SIG_IGN);
var_dump(posix_setrlimit(POSIX_RLIMIT_FSIZE, 0, POSIX_RLIMIT_INFINITY));

session_start();
$_SESSION['data'] = str_repeat('B', 4096);
@session_write_close();

clearstatcache(true);
var_dump(file_get_contents($file) === $before);

ob_end_flush();
echo "done\n";
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
int(8207)
bool(true)
bool(true)
done
