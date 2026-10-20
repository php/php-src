--TEST--
stream_select(): more than FD_SETSIZE sockets in the write and except sets on Windows (each set grows)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
?>
--FILE--
<?php
/* The read, write and except sets each grow on demand, independently. Put more
 * than FD_SETSIZE sockets in all three at once: every socket is readable (data
 * pending) and writable, none has an exceptional condition. select() returns the
 * number of set entries across all three sets, so each socket counts twice. */
$target = PHP_FD_SETSIZE + 50;
$pairs = [];
$socks = [];
for ($i = 0; $i < $target; $i++) {
    $p = stream_socket_pair(STREAM_PF_INET, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    if ($p === false) {
        break;
    }
    fwrite($p[0], "x");
    $pairs[] = $p;
    $socks[] = $p[1];
}

$r = $socks;
$w = $socks;
$e = $socks;
$ready = stream_select($r, $w, $e, 2, 0);

var_dump(count($socks) === $target);
var_dump($ready === 2 * $target);
var_dump(count($r) === $target, count($w) === $target, count($e));

/* A write set alone beyond FD_SETSIZE. */
$r = $e = null;
$w = $socks;
var_dump(stream_select($r, $w, $e, 2, 0) === $target);

foreach ($pairs as $p) {
    fclose($p[0]);
    fclose($p[1]);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
