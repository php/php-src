--TEST--
Uninitialized PDO objects
--SKIPIF--
<?php if (!extension_loaded('pdo')) die('skip'); ?>
--FILE--
<?php

class MyPDO extends PDO {
    public function __construct() {}
}
class MyPDOStatement extends PDOStatement {
    public function __construct() {}
}

$pdo = new MyPDO;
try {
    $pdo->query("foo");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$stmt = new MyPDOStatement;
try {
    $stmt->fetch();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
$stmt = new MyPDOStatement;
try {
    foreach ($stmt as $row) {}
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
PDO object is not initialized, constructor was not called
PDO object is uninitialized
PDO object is uninitialized
