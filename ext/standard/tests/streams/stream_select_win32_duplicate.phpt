--TEST--
stream_select(): a socket listed more than once is counted once on Windows
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
?>
--FILE--
<?php
/* On Windows the fd_set is an array of SOCKETs, so a socket listed more than
 * once in a set used to be stored - and counted by select() - that many times,
 * unlike the POSIX bitset where setting the same descriptor twice is a no-op.
 * stream_select() must de-duplicate, so the ready count matches POSIX. */
[$a, $b] = stream_socket_pair(STREAM_PF_INET, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
fwrite($b, "x"); // make $a readable

$r = [$a, $a, $a]; // the same socket three times
$w = $e = null;
var_dump(stream_select($r, $w, $e, 2, 0)); // ready count must be 1, not 3
var_dump(count($r)); // but all three entries are kept, as on POSIX

fclose($a);
fclose($b);
?>
--EXPECT--
int(1)
int(3)
