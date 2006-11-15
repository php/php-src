--TEST--
UTF-8<->ISO Latin 1 encoding/decoding test
--SKIPIF--
<?php include("skipif.inc"); ?>
--INI--
unicode.script_encoding=ISO-8859-1
unicode.output_encoding=ISO-8859-1
--FILE--
<?php
printf("%s -> %s\n", urlencode((binary)"æ"), urlencode(utf8_encode("æ")));
printf("%s <- %s\n", urlencode((binary)utf8_decode(urldecode((binary)"%C3%A6"))), (binary)"%C3%A6");
?>
--EXPECT--
%E6 -> %C3%A6
%E6 <- %C3%A6
