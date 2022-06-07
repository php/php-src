--TEST--
UTF-8<->ISO Latin 1 encoding/decoding test
--FILE--
<?php
printf("%s -> %s\n", urlencode("æ"), urlencode(utf8_encode("æ")));
printf("%s <- %s\n", urlencode(utf8_decode(urldecode("%C3%A6"))), "%C3%A6");
?>
--EXPECTF--
Deprecated: Function utf8_encode() is deprecated in %s on line %d
%E6 -> %C3%A6

Deprecated: Function utf8_decode() is deprecated in %s on line %d
%E6 <- %C3%A6
