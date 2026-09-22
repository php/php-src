--TEST--
Bug GH-9590 001 (stream_select works correctly, without warning, past the traditional
FD_SETSIZE descriptor limit on platforms where that limit has been lifted)
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists('posix_setrlimit') || !posix_setrlimit(POSIX_RLIMIT_NOFILE, 2048, -1)) {
    die('skip Failed to set POSIX_RLIMIT_NOFILE');
}
if (PHP_OS_FAMILY === 'Solaris' && PHP_INT_SIZE === 8)
    die('skip Solaris LP64 FD_SETSIZE=65536 not practically exceedable here');
?>
--FILE--
<?php

posix_setrlimit(POSIX_RLIMIT_NOFILE, 2048, -1);

$fds = [];
for ($i = 0; $i < 1023; $i++) {
    $fds[] = @fopen(__DIR__ . "/GH-9590-001-tmpfile.$i", 'w');
}

list($a, $b) = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
fwrite($b, "x");

$r = [$a];
$w = $e = [];
var_dump(stream_select($r, $w, $e, 30));
var_dump(fread($a, 1));

?>
--EXPECT--
int(1)
string(1) "x"
--CLEAN--
<?php
for ($i = 0; $i < 1023; $i++) {
    @unlink(__DIR__ . "/GH-9590-001-tmpfile.$i");
}
?>
