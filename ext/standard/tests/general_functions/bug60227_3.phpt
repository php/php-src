--TEST--
Bug #60227 (header() cannot detect the multi-line header with CR), \0 before \n
--INI--
expose_php=0
default_charset=UTF-8
--FILE--
<?php
header("X-Foo6: e\0Set-Cookie: ID=\n123\n d");
echo 'foo';
?>
--EXPECTF--
Warning: Header may not contain NUL bytes in %s on line %d
foo
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
