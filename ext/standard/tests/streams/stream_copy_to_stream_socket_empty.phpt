--TEST--
stream_copy_to_stream() from a socket already at EOF returns 0, not false
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip not for Windows');
?>
--FILE--
<?php
$file = tempnam(sys_get_temp_dir(), 'sct');
$dest = fopen($file, 'wb');

[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
fclose($b); // peer closed without sending; $a is at EOF

$copied = stream_copy_to_stream($a, $dest);
var_dump($copied);

fclose($a);
fclose($dest);
@unlink($file);
?>
--EXPECT--
int(0)
