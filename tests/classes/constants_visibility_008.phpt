--TEST--
Defined on private constant should not raise exception
--FILE--
<?php

class Foo
{
    private const BAR = 1;
}
echo (int)defined('Foo::BAR');
?>
--EXPECT--
0
