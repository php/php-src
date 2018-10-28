--TEST--
Bug #76754 (parent private constant in extends class memory leak)
--INI--
opcache.enable=0
opcache.enable_cli=0
--FILE--
<?php

class FOO
{
	private const FOO = 'BAR';
}

class BAR extends FOO { }
?>
okey
--EXPECT--
okey
