--TEST--
Bug #60227 (header() cannot detect the multi-line header with CR), \0 before \n
--FILE--
<?php
header("X-foo: e\n foo");
header("X-Foo6: e\0Set-Cookie: ID=\n123\n d");
echo 'foo';
?>
--EXPECTF--
Warning: Header may not contain NUL bytes in %s on line %d
foo
--EXPECTHEADERS--
X-foo: e
foo
