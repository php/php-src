--TEST--
Testing stream_get_meta_data() "timed_out" field on a udp socket
--FILE--
<?php

/* Setup socket server */
$server = stream_socket_server('tcp://127.0.0.1:31337');

/* Connect to it */
$client = fsockopen('tcp://127.0.0.1:31337');

/* Accept that connection */
$socket = stream_socket_accept($server);

var_dump(stream_get_meta_data($client));

echo "\n\nSet a timeout on the client and attempt a read:\n";
socket_set_timeout($client, 0, 1000);
fread($client, 1);
var_dump(stream_get_meta_data($client));

echo "\n\nWrite some data from the server:\n";
fwrite($socket, "12345");
var_dump(stream_get_meta_data($client));

echo "\n\nRead some data from the client:\n";
fread($client, 5);
var_dump(stream_get_meta_data($client));

fclose($client);
fclose($socket);
fclose($server);

?>
--EXPECTF--
array(8) {
  [u"stream_type"]=>
  unicode(%d) "tcp_socke%s"
  [u"mode"]=>
  unicode(2) "r+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(false)
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}


Set a timeout on the client and attempt a read:
array(8) {
  [u"stream_type"]=>
  unicode(%d) "tcp_socke%s"
  [u"mode"]=>
  unicode(2) "r+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(false)
  [u"timed_out"]=>
  bool(true)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}


Write some data from the server:

Notice: fwrite(): 5 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
array(8) {
  [u"stream_type"]=>
  unicode(%d) "tcp_socke%s"
  [u"mode"]=>
  unicode(2) "r+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(false)
  [u"timed_out"]=>
  bool(true)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}


Read some data from the client:
array(8) {
  [u"stream_type"]=>
  unicode(%d) "tcp_socke%s"
  [u"mode"]=>
  unicode(2) "r+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(false)
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
