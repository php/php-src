--TEST--
GH-7781 (Built-in server startup warnings should not contain HTML)
--FILE--
<?php
$process = proc_open(
    [PHP_BINARY, '-n', '-S', '0.0..0:8080'],
    [
        ['pipe', 'r'],
        ['pipe', 'w'],
        ['pipe', 'w'],
    ],
    $pipes,
);

fclose($pipes[0]);
$stdout = stream_get_contents($pipes[1]);
$stderr = stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
$status = proc_close($process);

var_dump($status);
var_dump($stdout);
var_dump(str_contains($stderr, 'Failed to listen on 0.0..0:8080 (reason: php_network_getaddresses:'));
?>
--EXPECT--
int(1)
string(0) ""
bool(true)
