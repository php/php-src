--TEST--
GH-17346 (PDOStatement::$queryString "readonly" is not properly implemented)
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false != $dir) die('skip is driver independent');
?>
--FILE--
<?php
$stmt = new PDOStatement();
try {
    for ($i = 0; $i < 10; $i++) {
        $stmt->queryString = (string) $i;
    }
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($stmt);
?>
--EXPECTF--
Property queryString is read only
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(1) "0"
}
