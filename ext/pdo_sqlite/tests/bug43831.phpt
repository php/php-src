--TEST--
Bug #43831 ($this gets mangled when extending PDO with persistent connection)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

class Foo extends PDO {
    function __construct($dsn) {
        parent::__construct($dsn, null, null, array(PDO::ATTR_PERSISTENT => true));
    }
}

class Baz extends PDO {
    function __construct($dsn) {
        parent::__construct($dsn, null, null, array(PDO::ATTR_PERSISTENT => true));
    }
}

class Bar extends Baz {
    function quux() {
        echo get_class($this), "\n";
        $foo = new Foo("sqlite::memory:");
        echo get_class($this), "\n";
    }
}

$bar = new Bar("sqlite::memory:");
$bar->quux();


class MyPDO extends PDO {}

$bar = new PDO("sqlite::memory:", null, null, array(PDO::ATTR_PERSISTENT => true));
$baz = new MyPDO("sqlite::memory:", null, null, array(PDO::ATTR_PERSISTENT => true));

var_dump($bar);
unset($bar);
var_dump($baz);
var_dump($bar);


?>
--EXPECTF--
Bar
Bar
object(PDO)#%d (0) {
}
object(MyPDO)#%d (0) {
}

Warning: Undefined variable $bar in %s on line %d
NULL
