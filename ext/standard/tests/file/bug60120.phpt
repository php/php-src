--TEST--
Bug #60120 (proc_open hangs when data in stdin/out/err is getting larger or equal to 2048)
--SKIPIF--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
if (!$php) {
    die("skip No php executable defined\n");
}
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
?>
--FILE--
<?php

error_reporting(E_ALL);

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$cmd = $php . ' -r "\$in = file_get_contents(\'php://stdin\'); fwrite(STDOUT, \$in); fwrite(STDERR, \$in);"';
$descriptors = array(array('pipe', 'r'), array('pipe', 'w'), array('pipe', 'w'));
$stdin = str_repeat('*', 2049 );

$options = array_merge(array('suppress_errors' => true, 'bypass_shell' => false));
$process = proc_open($cmd, $descriptors, $pipes, getcwd(), array(), $options);

foreach ($pipes as $pipe) {
    stream_set_blocking($pipe, false);
}
$writePipes = array($pipes[0]);
$stdinLen = strlen($stdin);
$stdinOffset = 0;

unset($pipes[0]);

$procOutput = [];
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
        $written = fwrite($writePipes[0], substr($stdin, $stdinOffset), 8192);
        if (false !== $written) {
            $stdinOffset += $written;
        } else {
            die('Failed to write to pipe');
        }
        if ($stdinOffset >= $stdinLen) {
            fclose($writePipes[0]);
            $writePipes = null;
        }
    }

    foreach ($r as $pipe) {
        $type = array_search($pipe, $pipes);
        $data = fread($pipe, 8192);
        if (feof($pipe)) {
            fclose($pipe);
            unset($pipes[$type]);
        } elseif (false === $data) {
            die('Failed to read from pipe');
        } else {
            $procOutput[$type] = ($procOutput[$type] ?? '') . $data;
        }
    }
}
foreach ($procOutput as $output) {
    if ($output !== $stdin) {
        die('Output does not match input: ' . $output);
    }
}
echo "OK.";
?>
--EXPECT--
OK.
