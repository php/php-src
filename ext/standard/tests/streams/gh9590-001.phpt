--TEST--
Bug GH-9590 001 (stream_select does not abort upon exception or empty valid fd set)
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists('posix_setrlimit') || !posix_setrlimit(POSIX_RLIMIT_NOFILE, 2048, -1)) {
    die('skip Failed to set POSIX_RLIMIT_NOFILE');
}
?>
--FILE--
<?php

posix_setrlimit(POSIX_RLIMIT_NOFILE, 2048, -1);

$fds = [];
for ($i = 0; $i < 1023; $i++) {
    $fds[] = @fopen(__DIR__ . "/GH-9590-001-tmpfile.$i", 'w');
}

list($a, $b) = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);

$r = [$a];
$w = $e = [];
var_dump(stream_select($r, $w, $e, PHP_INT_MAX));

?>
--EXPECTF--
Warning: stream_select(): You MUST recompile PHP with a larger value of FD_SETSIZE.
It is set to 1024, but you have descriptors numbered at least as high as %d.
 --enable-fd-setsize=%d is recommended, but you may want to set it
to equal the maximum number of open files supported by your system,
in order to avoid seeing this error again at a later date. in %s on line %d
bool(false)
--CLEAN--
<?php
for ($i = 0; $i < 1023; $i++) {
    @unlink(__DIR__ . "/GH-9590-001-tmpfile.$i");
}
?>
