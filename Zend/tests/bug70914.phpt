--TEST--
Bug #70914 zend_throw_or_error() format string vulnerability
--SKIPIF--
<?php
if (!extension_loaded("pdo_sqlite")) {
    die("skip pdo_sqlite required");
}
?>
--FILE--
<?php

$db = new PDO('sqlite::memory:');
$st = $db->query('SELECT 1');
try {
    $re = $st->fetchObject('%Z');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
PDOStatement::fetchObject(): Argument #1 ($class) must be a valid class name, %Z given
