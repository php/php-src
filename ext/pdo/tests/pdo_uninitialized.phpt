--TEST--
Uninitialized PDO objects
--EXTENSIONS--
pdo
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
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$stmt = new MyPDOStatement;
try {
    $stmt->fetch();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
$stmt = new MyPDOStatement;
try {
    foreach ($stmt as $row) {}
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: MyPDO object is uninitialized
Error: MyPDOStatement object is uninitialized
Error: MyPDOStatement object is uninitialized
