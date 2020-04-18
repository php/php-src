--TEST--
Converted values shall be returned and not the original value upon property assignment
--FILE--
<?php

class Test {
    public int $i;
    public string $s;
}

$test = new Test;
var_dump($test->i = "42");
var_dump($test->s = 42);

?>
--EXPECT--
int(42)
string(2) "42"
