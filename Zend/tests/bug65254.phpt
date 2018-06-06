--TEST--
Bug #65254 (Exception not catchable when exception thrown in autoload with a namespace)
--FILE--
<?php
spl_autoload_register(function ($class) {
    eval("namespace ns_test; class test {}");

    throw new \Exception('abcd');
});

try
{
    \ns_test\test::go();
}
catch (Exception $e)
{
    echo 'caught';
}
--EXPECT--
caught
