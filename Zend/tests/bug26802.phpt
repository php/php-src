--TEST--
Bug #26802 (Can't call static method using a variable)
--FILE--
<?php

function global_func()
{
    echo __METHOD__ . "\n";
}

$function = 'global_func';
$function();

class foo
{
    static $method = 'global_func';

    static public function foo_func()
    {
        echo __METHOD__ . "\n";
    }
}

/* The following is a BC break with PHP 4 where it would
 * call foo::fail. In PHP 5 we first evaluate static class
 * properties and then do the function call.
 */
$method = 'foo_func';
foo::$method();


?>
--EXPECT--
global_func
foo::foo_func
