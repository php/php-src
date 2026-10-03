--TEST--
Socket stream reads report timed_out only when the timeout expired
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip no AF_UNIX socket pair on Windows');
?>
--FILE--
<?php
[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_timeout($a, 5);

/* fread() returns the buffered rest without waiting for more */
fwrite($b, "a\nbcd");
var_dump(fgets($a));
var_dump(fread($a, 100), stream_get_meta_data($a)['timed_out']);

/* A zero timeout does not wait */
stream_set_timeout($a, 0);
var_dump(fread($a, 10), stream_get_meta_data($a)['timed_out']);

/* Negative microseconds count against the seconds */
fwrite($b, "x");
stream_set_timeout($a, 1, -5);
var_dump(fread($a, 10));
stream_set_timeout($a, 0, -5);
var_dump(fread($a, 10), stream_get_meta_data($a)['timed_out']);

stream_set_timeout($a, 0, 1000);
var_dump(fread($a, 10), stream_get_meta_data($a)['timed_out']);
?>
--EXPECT--
string(2) "a
"
string(3) "bcd"
bool(false)
string(0) ""
bool(false)
string(1) "x"
bool(false)
bool(true)
bool(false)
bool(true)
