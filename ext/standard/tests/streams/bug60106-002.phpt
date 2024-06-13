--TEST--
Bug #60106 (stream_socket_server with abstract unix socket paths)
--SKIPIF--
<?php
if (PHP_OS != "Linux") die("skip Only for Linux systems");
?>
--FILE--
<?php
error_reporting(E_ALL | E_NOTICE);

/* This figures out the max length for normal sockets */
$socket_file = "/tmp/" . str_repeat("a", 512);
function get_truncated_socket_filename($errno, $errmsg, $file, $line) {
    global $socket_file, $max_normal_length;
    echo $errmsg, "\n";
    preg_match("#maximum allowed length of (\d+) bytes#", $errmsg, $matches);
    $max_normal_length = intval($matches[1]);
    $socket_file = substr($socket_file, 0, $max_normal_length);
}
set_error_handler("get_truncated_socket_filename", E_NOTICE);

stream_socket_server("unix://" . $socket_file);
unlink($socket_file);

/* No we create an abstract one, prefixed with \0 so this should now work */
$abstract_socket = "\0" . $socket_file;
stream_socket_server("unix://" . $abstract_socket);

$old_max_length = $max_normal_length;

/* And now one longer, which should fail again */
$abstract_socket_long = "\0" . $abstract_socket . 'X';
stream_socket_server("unix://" . $abstract_socket_long);

echo "Allowed length is now one more: ", $max_normal_length == $old_max_length + 1 ? "yes" : "no", "\n";
?>
--EXPECTF--
stream_socket_server(): socket path exceeded the maximum allowed length of %d bytes and was truncated
stream_socket_server(): socket path exceeded the maximum allowed length of %d bytes and was truncated
Allowed length is now one more: yes
