--TEST--
PDO::ATTR_STATEMENT_CLASS with wrong-case class name fails with wrong case
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php
class MyStatement extends PDOStatement {
    public int $counter = 0;
}

$pdo = new PDO("sqlite::memory:");
$pdo->setAttribute(PDO::ATTR_STATEMENT_CLASS, ["MYSTATEMENT"]);
$stmt = $pdo->query("SELECT 1");
echo get_class($stmt) . "\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: PDO::setAttribute(): Argument #2 ($value) PDO::ATTR_STATEMENT_CLASS class must be a valid class in %s:%d
Stack trace:
#0 %s(7): PDO->setAttribute(13, Array)
#1 {main}
  thrown in %s on line %d
