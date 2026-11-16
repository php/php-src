--TEST--
GH-25299: Exception::__wakeup() keeps message and code held by reference
--FILE--
<?php

class MyException extends Exception {
    public $m = 'm';
    public $c = 5;

    public function __construct() {
        $this->message = &$this->m;
        $this->code = &$this->c;
    }
}

$e = unserialize(serialize(new MyException()));
var_dump($e->getMessage(), $e->getCode());

?>
--EXPECT--
string(1) "m"
int(5)
