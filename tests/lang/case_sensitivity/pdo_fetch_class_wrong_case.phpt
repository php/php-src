--TEST--
PDOStatement::setFetchMode(FETCH_CLASS) with wrong-case class name fails with wrong case
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php
class MyRow {
    public mixed $a = null;
}

$pdo = new PDO("sqlite::memory:");
$pdo->exec("CREATE TABLE t (a TEXT)");
$pdo->exec("INSERT INTO t VALUES ('hello')");

$stmt = $pdo->prepare("SELECT a FROM t");
$stmt->execute();
$stmt->setFetchMode(PDO::FETCH_CLASS, "MYROW");
$row = $stmt->fetch();
echo get_class($row) . "\n";
echo $row->a . "\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: PDOStatement::setFetchMode(): Argument #2 must be a valid class in %s:%d
Stack trace:
#0 %s(12): PDOStatement->setFetchMode(8, 'MYROW')
#1 {main}
  thrown in %s on line %d
