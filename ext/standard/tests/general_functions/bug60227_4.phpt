--TEST--
Bug #60227 (header() cannot detect the multi-line header with CR), CRLF
--INI--
expose_php=0
default_charset=UTF-8
--FILE--
<?php
header("X-foo: e\r\nfoo");
echo 'foo';
?>
--EXPECTF--
Warning: Header may not contain more than a single header, new line detected in %s on line %d
foo
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
