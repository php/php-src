--TEST--
Bug GH-9590 003 (stream_select past FD_SETSIZE: every set grows, and a stream in several sets is counted once per set)
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

// A low descriptor, opened before the filler.
list($lo, $lo_peer) = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);

$fds = [];
for ($i = 0; $i < 1023; $i++) {
    $fds[] = @fopen(__DIR__ . "/GH-9590-003-tmpfile.$i", 'w');
}

// A descriptor beyond FD_SETSIZE.
list($hi, $hi_peer) = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);

fwrite($lo_peer, "y");
fwrite($hi_peer, "x");

// $hi is in all three sets and $lo in two, so the read, write and except sets
// each have to grow past FD_SETSIZE. select() returns the number of bits set
// across all sets: $lo and $hi readable (2) + $hi writable (1) = 3.
$r = ['lo' => $lo, 'hi' => $hi];
$w = ['hi' => $hi];
$e = ['hi' => $hi, 'lo' => $lo];
var_dump(stream_select($r, $w, $e, 30));
var_dump(array_keys($r), array_keys($w), $e);

// A beyond-FD_SETSIZE stream in the write set alone.
$r = $e = null;
$w = [$hi];
var_dump(stream_select($r, $w, $e, 30));

?>
--EXPECT--
int(3)
array(2) {
  [0]=>
  string(2) "lo"
  [1]=>
  string(2) "hi"
}
array(1) {
  [0]=>
  string(2) "hi"
}
array(0) {
}
int(1)
--CLEAN--
<?php
for ($i = 0; $i < 1023; $i++) {
    @unlink(__DIR__ . "/GH-9590-003-tmpfile.$i");
}
?>
