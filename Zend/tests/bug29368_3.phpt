--TEST--
Bug #29368.3 (The destructor is called when an exception is thrown from the constructor).
--FILE--
<?php
class Foo {
    function __construct() {
        echo __METHOD__ . "\n";
    }
    function __destruct() {
        echo __METHOD__ . "\n";
    }
}
class Bar {
    function __construct() {
        echo __METHOD__ . "\n";
        throw new Exception;
    }
    function __destruct() {
        echo __METHOD__ . "\n";
    }
}

try {
    new Foo() + new Bar();
} catch(Exception $exc) {
    echo "Caught exception!\n";
}
?>
--EXPECT--
Foo::__construct
Bar::__construct
Foo::__destruct
Caught exception!
