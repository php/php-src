--TEST--
Bug #60227 (header() cannot detect the multi-line header with CR)
--FILE--
<?php
header("X-Foo1: a");
header("X-Foo2: b\n ");
header("X-Foo3: c\r\n ");
header("X-Foo4: d\r ");
header("X-Foo5: e\rSet-Cookie: ID=123");
echo 'foo';
?>
--EXPECTF--
Warning: Header may not contain more than a single header, new line detected in %s on line %d
foo
--EXPECTHEADERS--
X-Foo1: a
X-Foo2: b
X-Foo3: c
X-Foo4: d

