--TEST--
GH-24506 (stream_select() leaks stream error operations on FD_SETSIZE failure)
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (PHP_FD_SETSIZE > 4096) {
    die('skip FD_SETSIZE is too large');
}
if (!function_exists('posix_setrlimit')
    || !posix_setrlimit(POSIX_RLIMIT_NOFILE, PHP_FD_SETSIZE + 32, PHP_FD_SETSIZE + 32)) {
    die('skip Failed to set POSIX_RLIMIT_NOFILE');
}
?>
--FILE--
<?php
posix_setrlimit(POSIX_RLIMIT_NOFILE, PHP_FD_SETSIZE + 32, PHP_FD_SETSIZE + 32);

$streams = [];
for ($i = 0; $i < PHP_FD_SETSIZE; $i++) {
    $streams[] = fopen(__FILE__, 'r');
}
[$reader, $writer] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);

$warnings = 0;
set_error_handler(static function (int $errno, string $message) use (&$warnings): bool {
    if (str_contains($message, 'larger value of FD_SETSIZE')) {
        $warnings++;
        return true;
    }
    return false;
});

$failures = 0;
for ($i = 0; $i < 1001; $i++) {
    $read = [$reader];
    $write = $except = null;
    if (stream_select($read, $write, $except, 0) === false) {
        $failures++;
    }
}
restore_error_handler();

var_dump($warnings, $failures);
var_dump(fwrite($writer, 'x'));
var_dump(fread($reader, 1));
?>
--EXPECT--
int(1001)
int(1001)
int(1)
string(1) "x"
