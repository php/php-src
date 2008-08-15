--TEST--
Testing stream_get_meta_data() "eof" field on a udp socket
--FILE--
<?php

/* Setup socket server */
$server = stream_socket_server('tcp://127.0.0.1:31337');
if ($server === false) {
	die("stream_socket_server() failed\n");
}

/* Connect to it */
$client = fsockopen('tcp://127.0.0.1:31337');
if ($client === false) {
	die("fsockopen() failed\n");
}

/* Accept that connection */
$socket = stream_socket_accept($server);
if ($socket === false) {
	die("stream_socket_accept() failed\n");
}

echo "Write some data:\n";
fwrite($socket, "abcdefg\n1234567\nxyzxyz\n");
var_dump(stream_get_meta_data($client));

echo "\n\nRead a line from the client:\n";
fgets($client);
var_dump(stream_get_meta_data($client));

echo "\n\nClose the server side socket and read the remaining data from the client:\n";
fclose($socket);
fclose($server);
while(!feof($client)) {
	fread($client, 1);
}
var_dump(stream_get_meta_data($client));

fclose($client);

?>
--EXPECTF--
Write some data:

Notice: fwrite(): 23 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
array(8) {
  [u"stream_type"]=>
  unicode(%d) "tcp_socke%s"
  [u"mode"]=>
  unicode(2) "r+"
  [u"unread_bytes"]=>
  int(%i)
  [u"unread_chars"]=>
  int(%i)
  [u"seekable"]=>
  bool(false)
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}


Read a line from the client:
array(8) {
  [u"stream_type"]=>
  unicode(%d) "tcp_socke%s"
  [u"mode"]=>
  unicode(2) "r+"
  [u"unread_bytes"]=>
  int(%i)
  [u"unread_chars"]=>
  int(%i)
  [u"seekable"]=>
  bool(false)
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}


Close the server side socket and read the remaining data from the client:
array(8) {
  [u"stream_type"]=>
  unicode(%d) "tcp_socke%s"
  [u"mode"]=>
  unicode(2) "r+"
  [u"unread_bytes"]=>
  int(%i)
  [u"unread_chars"]=>
  int(%i)
  [u"seekable"]=>
  bool(false)
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(true)
}
