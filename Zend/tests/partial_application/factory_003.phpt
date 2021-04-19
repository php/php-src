--TEST--
Partial application factory: exception
--FILE--
<?php
class Foo {
    public function __construct() {
        throw new Exception("boo");
    }

    public function __destruct() {
        printf("%s\n", __METHOD__);
    }
}

$foo = new Foo(...);

try {
    $foo();
} catch (Exception $ex) {
    printf("%s\n", $ex->getMessage());
}
?>
--EXPECT--
boo
