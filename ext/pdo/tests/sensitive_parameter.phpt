--TEST--
Test that sensitive parameters are marked sensitive.
--EXTENSIONS--
pdo
--FILE--
<?php
try {
    new PDO('dsn', 'username', 'password');
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
?>
--EXPECTF--
PDOException: PDO::__construct(): Argument #1 ($dsn) must be a valid data source name in %s:%d
Stack trace:
#0 %s(%d): PDO->__construct('dsn', 'username', Object(SensitiveParameterValue))
#1 {main}
