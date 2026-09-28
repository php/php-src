--TEST--
stream_socket_recvfrom() on a blocking stream is not bound by default_socket_timeout
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip no proc_open');
?>
--INI--
default_socket_timeout=1
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$args = getenv('TEST_PHP_EXTRA_ARGS');

$s = stream_socket_server('udp://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND);
$name = stream_socket_get_name($s, false);
$code = '<?php usleep(2000000); fwrite(stream_socket_client("udp://' . $name . '"), "hello");';
// A script file: the code has quotes of its own, which escapeshellarg() cannot carry on Windows
$script = tempnam(sys_get_temp_dir(), 'recvfrom');
file_put_contents($script, $code);
$p = proc_open("$php $args " . escapeshellarg($script), [], $pipes);
var_dump(stream_socket_recvfrom($s, 10));
proc_close($p);
unlink($script);
?>
--EXPECT--
string(5) "hello"
