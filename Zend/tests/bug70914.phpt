--TEST--
Bug #70914 zend_throw_or_error() format string vulnerability
--EXTENSIONS--
pdo_sqlite
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
