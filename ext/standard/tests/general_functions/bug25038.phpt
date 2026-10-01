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
catch (Throwable $e)
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try
{
    call_user_func('bar','second try');
}
catch (Throwable $e)
{
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: This is an exception from bar(first try).
Exception: This is an exception from bar(second try).
