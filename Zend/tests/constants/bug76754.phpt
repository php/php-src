--TEST--
Bug #76754 (parent private constant in extends class memory leak)
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
