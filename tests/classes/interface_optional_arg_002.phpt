--TEST--
default argument value in interface implementation
--FILE--
<?php

interface test {
    public function bar();
}

class foo implements test {

    public function bar($arg = 2) {
        var_dump($arg);
    }
}

$foo = new foo;
$foo->bar();

?>
--EXPECT--
int(2)
