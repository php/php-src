--TEST--
dtls:// stream transport is registered
--EXTENSIONS--
openssl
--FILE--
<?php
var_dump(in_array('dtls', stream_get_transports(), true));
?>
--EXPECT--
bool(true)
