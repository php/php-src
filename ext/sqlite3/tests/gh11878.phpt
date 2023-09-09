--TEST--
GH-11878 (SQLite3 callback functions cause a memory leak with a callable array)
--EXTENSIONS--
sqlite3
--FILE--
<?php
class Foo {
    public $sqlite;
    public function __construct(bool $normalFunctions, bool $aggregates) {
        $this->sqlite = new SQLite3(":memory:");
        if ($aggregates) {
            $this->sqlite->createAggregate("indexes", array($this, "SQLiteIndex"), array($this, "SQLiteFinal"), 0);
        }
        if ($normalFunctions) {
            $this->sqlite->createFunction("func", array($this, "SQLiteIndex"), 0);
            $this->sqlite->createCollation("collation", array($this, "SQLiteIndex"));
        }
    }
    public function SQLiteIndex() {}
    public function SQLiteFinal() {}
}

// Test different combinations to check for null pointer derefs
$x = new Foo(true, true);
$y = new Foo(false, true);
$z = new Foo(true, false);
$w = new Foo(false, false);
?>
Done
--EXPECT--
Done
