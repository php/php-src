--TEST--
An extension resolves any Io\Poll\Handle through php_poll_handle_from_zval()
--EXTENSIONS--
zend_test
--FILE--
<?php
$domain = 'WIN' === strtoupper(substr(PHP_OS, 0, 3)) ? STREAM_PF_INET : STREAM_PF_UNIX;
[$r, $w] = stream_socket_pair($domain, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);

$handle = new StreamPollHandle($r);
var_dump(zend_test_poll_handle_descriptor($handle) >= 0);

// anything that is not a handle resolves to nothing
var_dump(zend_test_poll_handle_descriptor($r));
var_dump(zend_test_poll_handle_descriptor(new stdClass()));
var_dump(zend_test_poll_handle_descriptor('nope'));

fclose($r);
fclose($w);
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
