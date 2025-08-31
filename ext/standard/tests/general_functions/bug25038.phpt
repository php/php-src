--TEST--
Bug #25038 (call_user_func issues warning if function throws exception)
--FILE--
<?php

function bar($x='no argument')
{
    throw new Exception("This is an exception from bar({$x}).");
}
try
{
    bar('first try');
}
catch (Exception $e)
{
    print $e->getMessage()."\n";
}
try
{
    call_user_func('bar','second try');
}
catch (Exception $e)
{
    print $e->getMessage()."\n";
}

?>
--EXPECT--
This is an exception from bar(first try).
This is an exception from bar(second try).
