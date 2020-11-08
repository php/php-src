--TEST--
__sleep() can return references
--FILE--
<?php

class Test {
    public $x = 42;
    public function __sleep() {
        $name = 'x';
        return [&$name];
    }
}

var_dump(serialize(new Test));

?>
--EXPECT--
string(28) "O:4:"Test":1:{s:1:"x";i:42;}"
