--TEST--
Non-promoted constructor parameter does not conflict with final promoted property
--FILE--
<?php

class A {
    public function __construct(
        final $prop
    ) {
        echo __METHOD__ . "(): $prop\n";
    }
}

class B extends A {
    public function __construct(
        $prop
    ) {
        echo __METHOD__ . "(): $prop\n";
        parent::__construct($prop);
    }
}

$b = new B("test");

?>
--EXPECT--
B::__construct(): test
A::__construct(): test
