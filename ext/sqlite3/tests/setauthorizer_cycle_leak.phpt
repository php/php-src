--TEST--
setAuthorizer() cycle leak
--EXTENSIONS--
sqlite3
--FILE--
<?php
class Foo extends Sqlite3 {
    public function __construct() {
        parent::__construct(":memory:");
        $this->setAuthorizer([$this, "foo"]);
    }

    public function foo() {}
}

$test = new Foo;

echo "Done\n";
?>
--EXPECT--
Done
