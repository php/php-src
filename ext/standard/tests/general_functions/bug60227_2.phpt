--TEST--
Bug #60227 (header() cannot detect the multi-line header with CR), \r before \n
--FILE--
<?php
header("X-foo: e\n foo");
header("X-Foo6: e\rSet-Cookie: ID=123\n d");
echo 'foo';
?>
--EXPECTF--
Warning: Header may not contain more than a single header, new line detected in %s on line %d
foo
--EXPECTHEADERS--
X-foo: e
foo
