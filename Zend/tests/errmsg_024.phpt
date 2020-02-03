--TEST--
No more errmsg: can now change initial value of property
--FILE--
<?php

class test1
{
    protected static $var = 1;
}

class test extends test1
{
    static $var = 10;
}

echo "Done\n";

?>
--EXPECT--
Done
