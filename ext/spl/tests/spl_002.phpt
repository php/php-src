--TEST--
SPL: Countable
--FILE--
<?php

class Test implements Countable
{
    function count(): int
    {
        return 4;
    }
};

$a = new Test;

var_dump(count($a));

?>
--EXPECT--
int(4)
