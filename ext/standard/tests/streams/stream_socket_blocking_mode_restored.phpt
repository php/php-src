--TEST--
Socket streams give a shared descriptor back in blocking mode
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip not for Windows');
if (!function_exists('proc_open')) die('skip no proc_open');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$args = getenv('TEST_PHP_EXTRA_ARGS');

/* The child PHP opens its socket stdout as a stream, then the shell reuses
 * the same socket for a writer that relies on blocking mode */
[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$cmd = "$php $args -r 'echo \"php\\n\";'; read x; head -c 1000000 /dev/zero";
$p = proc_open($cmd, [0 => ['pipe', 'r'], 1 => $b, 2 => ['pipe', 'w']], $pipes);
/* The parent's own stream on the same socket also gives it back blocking */
fclose($b);
var_dump(fgets($a));
fwrite($pipes[0], "go\n");
fclose($pipes[0]);
/* Let the writer fill the socket buffer before reading: in non-blocking
 * mode it fails with EAGAIN and reports it on stderr */
$r = [$pipes[2]]; $w = $e = null;
stream_select($r, $w, $e, 0, 500000);
$n = 0;
while (!feof($a)) {
    $n += strlen(fread($a, 65536));
}
var_dump($n, stream_get_contents($pipes[2]), proc_close($p));
?>
--EXPECT--
string(4) "php
"
int(1000000)
string(0) ""
int(0)
