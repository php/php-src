--TEST--
PDO Common: Bug #77849 (inconsistent state of cloned statament object)
--EXTENSIONS--
pdo
--FILE--
<?php

try {
    $stmt = new PDOStatement();
    clone $stmt;
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Trying to clone an uncloneable object of class PDOStatement
