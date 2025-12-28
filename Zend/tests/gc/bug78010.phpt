--TEST--
Bug #78010: Segmentation fault during GC
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--INI--
memory_limit=2G
--FILE--
<?php

#[AllowDynamicProperties]
class foo
{
    public function __construct()
    {
        $this->x = $this;

        for ($i = 0; $i < 898; $i++) { //Will not trigger with <898
            $obj = [new stdClass, new stdClass]; //This must have at least 2 elements
            $this->y[] = $obj;
        }
    }
}

for ($i = 0; $i < 2; ++$i) { //This must run >=2 (increasing the number of elements in the array *2 will not do)
    $x = []; //This must be reset
    foreach (array_fill(0, 389, 'x') as &$params) { //Will not trigger <389
        $x[] = new foo;
    }
}

echo "Completed\n";

?>
--EXPECT--
Completed
