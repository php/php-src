--TEST--
stream_select(): more than FD_SETSIZE sockets on Windows (growable fd_set)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
?>
--FILE--
<?php
/* On Windows the winsock fd_set is a packed array of SOCKETs capped at the
 * compile-time FD_SETSIZE (default 256, see --enable-fd-setsize). stream_select()
 * now grows the set on demand, so it can watch far more than FD_SETSIZE sockets,
 * matching POSIX behaviour. Here we select on FD_SETSIZE + 50 readable sockets
 * and expect every one to be reported ready; before the fix the winsock FD_SET
 * macro silently dropped everything past FD_SETSIZE. */
$target = PHP_FD_SETSIZE + 50;
$pairs = [];
$reads = [];
for ($i = 0; $i < $target; $i++) {
    $p = stream_socket_pair(STREAM_PF_INET, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    if ($p === false) {
        break;
    }
    fwrite($p[0], "x");
    $pairs[] = $p;
    $reads[] = $p[1];
}

$r = $reads;
$w = $e = null;
$ready = stream_select($r, $w, $e, 2, 0);

var_dump(count($reads) === $target);
var_dump($ready === $target);
var_dump(count($r) === $target);

foreach ($pairs as $p) {
    fclose($p[0]);
    fclose($p[1]);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
