--TEST--
passing arguments by ref to a method handled by __call()
--INI--
allow_call_time_pass_reference=1
--FILE--
<?php

class Foo {
    function __call($method, $args)
    {
        print $args[0]."\n";
        $args[0] = 5;
        print $args[0]."\n";
        return true;
    }
}

$v = 'str';
$o = new Foo();
$o->test(&$v);

var_dump($v);

echo "Done\n";
?>
--EXPECTF--	
str
5
int(5)
Done
