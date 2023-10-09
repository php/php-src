--TEST--
GH-10406: feof() behavior change for UNIX based socket resources
--SKIPIF--
<?php
if (!strncasecmp(PHP_OS, 'WIN', 3)) die("skip. Do not run on Windows");
?>
--FILE--
<?php
$socket_path = '/tmp/gh-10406-test.socket';

if (file_exists($socket_path)) {
    unlink($socket_path);
}

$socket = stream_socket_server('unix://' . $socket_path);
var_dump(feof($socket));
unlink($socket_path);
?>
--EXPECT--
bool(false)
