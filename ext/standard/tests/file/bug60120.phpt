--TEST--
Bug #60120 (proc_open hangs when data in stdin/out/err is getting larger or equal to 2048)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
}
$php = getenv('TEST_PHP_EXECUTABLE');
if (!$php) {
	die("No php executable defined\n");
}
?>
--FILE--
<?php

error_reporting(E_ALL);

$php = getenv('TEST_PHP_EXECUTABLE');
if (!$php) {
	die("No php executable defined\n");
}
$cmd = 'php -r "fwrite(STDOUT, $in = file_get_contents(\'php://stdin\')); fwrite(STDERR, $in);"';
$descriptors = array(array('pipe', 'r'), array('pipe', 'w'), array('pipe', 'w'));
$stdin = str_repeat('*', 1024 * 16) . '!';
$stdin = str_repeat('*', 2049 );

$options = array_merge(array('suppress_errors' => true, 'binary_pipes' => true, 'bypass_shell' => false));
$process = proc_open($cmd, $descriptors, $pipes, getcwd(), array(), $options);

foreach ($pipes as $pipe) {
    stream_set_blocking($pipe, false);
}
$writePipes = array($pipes[0]);
$stdinLen = strlen($stdin);
$stdinOffset = 0;

unset($pipes[0]);

while ($pipes || $writePipes) {
    $r = $pipes;
    $w = $writePipes;
    $e = null;
    $n = stream_select($r, $w, $e, 60);

    if (false === $n) {
        break;
    } elseif ($n === 0) {
        proc_terminate($process);

    }
    if ($w) {
        $written = fwrite($writePipes[0], (binary)substr($stdin, $stdinOffset), 8192);
        if (false !== $written) {
            $stdinOffset += $written;
        }
        if ($stdinOffset >= $stdinLen) {
            fclose($writePipes[0]);
            $writePipes = null;
        }
    }

    foreach ($r as $pipe) {
        $type = array_search($pipe, $pipes);
        $data = fread($pipe, 8192);
        if (false === $data || feof($pipe)) {
            fclose($pipe);
            unset($pipes[$type]);
        }
    }
}
echo "OK.";
?>
--EXPECT--
OK.
