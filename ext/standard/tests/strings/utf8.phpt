--TEST--
UTF-8<->ISO Latin 1 encoding/decoding test
--FILE--
<?php
printf("%s -> %s\n", urlencode("�"), urlencode(utf8_encode("�")));
printf("%s <- %s\n", urlencode(utf8_decode(urldecode("%C3%A6"))), "%C3%A6");
?>
--EXPECT--
%E6 -> %C3%A6
%E6 <- %C3%A6
