--TEST--
stream_get_line() on non-blocking stream
--SKIPIF--
<?php
$sockets = @stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
if (!$sockets) die("skip stream_socket_pair() should work");
fclose($sockets[0]);
fclose($sockets[1]);
?>
--FILE--
<?php
/**
 * Tests that stream_get_line() behaves as documented on non-blocking streams:
 * Never return incomplete lines, except on documented conditions:
 * length bytes have been read, the string specified by ending is found, EOF.
 */

$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
var_dump($sockets);

stream_set_blocking($sockets[1], 0);

$eol = b'<EOL>';

fwrite($sockets[0], b"line start");
var_dump(stream_get_line($sockets[1], 8192, $eol)); // Does not returns incomplete line (EOL not found)
var_dump(stream_get_line($sockets[1], 8192, $eol));
fwrite($sockets[0], b", line end");
fwrite($sockets[0], b", $eol");
var_dump(stream_get_line($sockets[1], 8192, $eol)); // Returns full line (EOL found)
var_dump(stream_get_line($sockets[1], 8192, $eol)); // Nothing to read
var_dump(stream_get_line($sockets[1], 8192, $eol));

fwrite($sockets[0], b"incomplete line");
var_dump(stream_get_line($sockets[1], strlen(b"incomplete line"), $eol)); // EOL not found but $length has been read, return incomplete line

fwrite($sockets[0], b"incomplete line");
var_dump(stream_get_line($sockets[1], 8192, $eol)); // Does not returns incomplete line (EOL not found)
var_dump(fread($sockets[1], strlen(b"incomplete line"))); // Returns buffer readden by stream_get_line

fwrite($sockets[0], b"end of file");
var_dump(stream_get_line($sockets[1], 8192, $eol)); // Does not returns incomplete line (EOL not found)

fclose($sockets[0]);
var_dump(stream_get_line($sockets[1], 8192, $eol)); // Returns incomplete line (End of file)

fclose($sockets[1]);

?>
--EXPECTF--
array(2) {
  [0]=>
  resource(%d) of type (stream)
  [1]=>
  resource(%d) of type (stream)
}
bool(false)
bool(false)
string(22) "line start, line end, "
bool(false)
bool(false)
string(15) "incomplete line"
bool(false)
string(15) "incomplete line"
bool(false)
string(11) "end of file"
