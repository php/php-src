--TEST--
GH-11878 (SQLite3 callback functions cause a memory leak with a callable array)
--EXTENSIONS--
sqlite3
--FILE--
<?php
class Foo {
    public $sqlite;
    public function __construct() {
        $this->sqlite = new SQLite3(":memory:");
        $this->sqlite->createAggregate("indexes", array($this, "SQLiteIndex"), array($this, "SQLiteFinal"), 0);
        $this->sqlite->createFunction("func", array($this, "SQLiteIndex"), 0);
        $this->sqlite->createCollation("collation", array($this, "SQLiteIndex"));
    }
    public function SQLiteIndex() {}
    public function SQLiteFinal() {}
}

$x = new Foo;
?>
Done
--EXPECT--
Done
