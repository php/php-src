--TEST--
Bug #36006 (Problem with $this in __destruct())
--FILE--
<?php

class Person {
    public $dad;
    public function __destruct() {
        $this->dad = null; /* no segfault if this is commented out */
    }
}

class Dad extends Person {
    public $son;
    public function __construct() {
        $this->son = new Person;
        $this->son->dad = $this; /* no segfault if this is commented out */
    }
    public function __destruct() {
        $this->son = null;
        parent::__destruct(); /* segfault here */
    }
}

$o = new Dad;
unset($o);
echo "ok\n";
?>
--EXPECT--
ok
