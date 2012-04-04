--TEST--
Bug #60227 (header() cannot detect the multi-line header with CR), CRLF
--FILE--
<?php
header("X-foo: e\r\n foo");
header("X-foo: e\r\nfoo");
echo 'foo';
?>
--EXPECTF--
Warning: Header may not contain more than a single header, new line detected in %s on line %d
foo
--EXPECTHEADERS--
X-foo: e
 foo
