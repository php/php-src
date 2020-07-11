--TEST--
SPL: RoRewindIterator and string keys
--FILE--
<?php

foreach(new NoRewindIterator(new ArrayIterator(array('Hello'=>0, 'World'=>1))) as $k => $v)
{
    var_dump($v);
    var_dump($k);
}

?>
--EXPECT--
int(0)
string(5) "Hello"
int(1)
string(5) "World"
