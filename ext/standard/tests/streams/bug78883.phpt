--TEST--
Bug #78883 (fgets(STDIN) fails on Windows)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip this test is for Windows platforms only');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
$descriptorspec = array(
   0 => array("pipe", "rb"),
   1 => array("pipe", "wb"),
   //2 => array("file", "stderr.txt", "ab")
);
$pipes = [];
$cmd = 'cmd.exe "/c START ^"^" /WAIT ' . getenv('TEST_PHP_EXECUTABLE_ESCAPED') . ' -r ^"var_dump(fgets(STDIN));"';
$proc = proc_open($cmd, $descriptorspec, $pipes);
var_dump(is_resource($proc));
$pid = proc_get_status($proc)['pid'];
sleep(3);
$bug_is_present = !proc_get_status($proc)['running'];
if (!$bug_is_present) {
    // if the bug is not present, it will hang waiting for stdin,
    // thus cmd is still running and we should kill it
    shell_exec("taskkill /T /F /PID {$pid} 2>nul");
}
fclose($pipes[0]);
fclose($pipes[1]);
proc_close($proc);
var_dump($bug_is_present);
?>
--EXPECT--
bool(true)
bool(false)
