--TEST--
Edge case: self::self, self::parent, parent::self semi reserved constants access
--FILE--
<?php

class Foo {
    const self = "self";
    const parent = "parent";
    public function __construct() {
        echo "From ", __METHOD__, ":", PHP_EOL;
        echo self::self, PHP_EOL;
        echo self::parent, PHP_EOL;
    }
}

class Bar extends Foo {
    public function __construct() {
        parent::__construct();
        echo "From ", __METHOD__, ":", PHP_EOL;
        echo parent::self, PHP_EOL;
        echo parent::parent, PHP_EOL;
    }
}

new Bar;

echo "\nDone\n";
--EXPECTF--
From Foo::__construct:
self
parent
From Bar::__construct:
self
parent

Done
