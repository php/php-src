--TEST--
Bug #64438 proc_open hangs with stdin/out with 4097+ bytes
--FILE--
<?php

error_reporting(E_ALL);

if (substr(PHP_OS, 0, 3) == 'WIN') {
	$cmd = PHP_BINARY . ' -n -r "fwrite(STDOUT, $in = file_get_contents(\'php://stdin\')); fwrite(STDERR, $in);"';
} else {
	$cmd = PHP_BINARY . ' -n -r \'fwrite(STDOUT, $in = file_get_contents("php://stdin")); fwrite(STDERR, $in);\'';
}
$descriptors = array(array('pipe', 'r'), array('pipe', 'w'), array('pipe', 'w'));
$stdin = str_repeat('*', 4097);

$options = array_merge(array('suppress_errors' => true, 'bypass_shell' => false));
$process = proc_open($cmd, $descriptors, $pipes, getcwd(), array(), $options);

foreach ($pipes as $pipe) {
    stream_set_blocking($pipe, false);
}
$writePipes = array($pipes[0]);
$stdinLen = strlen($stdin);
$stdinOffset = 0;

unset($pipes[0]);

$pipeEvents = [];
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
            $pipeEvents[(int)$pipe][] = "Closing pipe";
            fclose($pipe);
            unset($pipes[$type]);
        } else {
            $pipeEvents[(int)$pipe][] = "Read " . strlen($data) . " bytes";
        }
    }
}

var_dump($pipeEvents);

?>
===DONE===
--EXPECTF--
array(2) {
  [%d]=>
  array(2) {
    [0]=>
    string(15) "Read 4097 bytes"
    [1]=>
    string(12) "Closing pipe"
  }
  [%d]=>
  array(2) {
    [0]=>
    string(15) "Read 4097 bytes"
    [1]=>
    string(12) "Closing pipe"
  }
}
===DONE===
