--TEST--
Testing socket_get_status()
--FILE--
<?php

$tcp_socket = stream_socket_server('tcp://127.0.0.1:31337');
var_dump(socket_get_status($tcp_socket));
fclose($tcp_socket);

?>
--EXPECTF--
array(8) {
  [u"stream_type"]=>
  unicode(%d) "tcp_socket%S"
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
