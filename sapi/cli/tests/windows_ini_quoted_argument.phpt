--TEST--
Windows CLI INI options preserve CRT doubled-quote escaping
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
?>
--FILE--
<?php
$code = "echo bin2hex(ini_get('user_agent')), PHP_EOL;";
$command = escapeshellarg(PHP_BINARY) . ' -n -r ' . escapeshellarg($code)
    . ' -d "user_agent=""hello world"""';
$process = proc_open($command, [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes,
    null, null, ['bypass_shell' => true]);
echo stream_get_contents($pipes[1]);
fclose($pipes[1]);
var_dump(proc_close($process));
?>
--EXPECT--
68656c6c6f20776f726c64
int(0)
