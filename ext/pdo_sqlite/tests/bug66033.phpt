--TEST--
Bug #66033 (Segmentation Fault when constructor of PDO statement throws an exception)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
class DBStatement extends PDOStatement {
    public $dbh;
    protected function __construct($dbh) {
        $this->dbh = $dbh;
        throw new Exception("Blah");
    }
}

$pdo = new PDO('sqlite::memory:', null, null);
$pdo->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('DBStatement',
    array($pdo)));
$pdo->exec("CREATE TABLE IF NOT EXISTS messages (
    id INTEGER PRIMARY KEY,
    title TEXT,
    message TEXT,
    time INTEGER)");

try {
    $pdoStatement = $pdo->query("select * from messages");
} catch (Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(4) "Blah"
