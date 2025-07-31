--TEST--
Bug #71724: yield from does not count EOLs
--FILE--
<?php

function test()
{
    yield




    from [__LINE__];
}
var_dump(test()->current());

?>
--EXPECT--
int(10)
