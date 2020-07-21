--TEST--
proc_open() with non-blocking pipes (threaded on Windows)
--FILE--
<?php

function poll($pipe, $read = true)
{
	$r = ($read == true) ? [$pipe] : null;
	$w = ($read == false) ? [$pipe] : null;
	$e = null;
	
	if (!stream_select($r, $w, $e, null, 0)) {
		throw new \Error("Select failed");
	}
}

function read_pipe($pipe): string
{
	poll($pipe);
	
	if (false === ($chunk = @fread($pipe, 8192))) {
		throw new Error("Failed to read: " . (error_get_last()['message'] ?? 'N/A'));
	}
	
	return $chunk;
}

function write_pipe($pipe, $data)
{
	poll($pipe, false);
	
	if (false == @fwrite($pipe, $data)) {
		throw new Error("Failed to write: " . (error_get_last()['message'] ?? 'N/A'));
	}
}

$cmd = [
	getenv("TEST_PHP_EXECUTABLE"),
	__DIR__ . '/proc_open_sockets2.inc'
];

$spec = [
	['pipe', 'rb'],
	['pipe', 'wb']
];

$proc = proc_open($cmd, $spec, $pipes, null, null, [
	'threaded_pipes' => true
]);

foreach ($pipes as $pipe) {
	var_dump(stream_set_blocking($pipe, false));
}

printf("STDOUT << %s\n", read_pipe($pipes[1]));
printf("STDOUT << %s\n", read_pipe($pipes[1]));

write_pipe($pipes[0], 'done');
fclose($pipes[0]);

var_dump(feof($pipes[1]));
printf("STDOUT << %s\n", read_pipe($pipes[1]));
var_dump(read_pipe($pipes[1]));
var_dump(feof($pipes[1]));

?>
--EXPECTF--
bool(true)
bool(true)
STDOUT << hello
STDOUT << world
bool(false)
STDOUT << DONE
string(0) ""
bool(true)
