--TEST--
PDO::ATTR_STATEMENT_CLASS: destructors of old constructor arguments do not observe them being released
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
class St extends PDOStatement {
    protected function __construct(...$args) {}
}
class D {
    public function __destruct() {
        global $db;
        $GLOBALS['keep'] = $db->getAttribute(PDO::ATTR_STATEMENT_CLASS);
    }
}
$db = new PDO('sqlite::memory:');
$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['St', [str_repeat('A', 64), new D, str_repeat('B', 64)]]);
$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, ['St', [1]]);
var_dump($keep);
?>
--EXPECT--
array(2) {
  [0]=>
  string(2) "St"
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
}
