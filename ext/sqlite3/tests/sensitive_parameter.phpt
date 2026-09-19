--TEST--
Test that the encryption key parameter of SQLite3::__construct() and SQLite3::open() is marked sensitive.
--EXTENSIONS--
sqlite3
--FILE--
<?php
try {
    new SQLite3(':memory:', 'not-an-int', 'secret-key');
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
try {
    $db = (new ReflectionClass(SQLite3::class))->newInstanceWithoutConstructor();
    $db->open(':memory:', 'not-an-int', 'secret-key');
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
?>
--EXPECTF--
TypeError: SQLite3::__construct(): Argument #2 ($flags) must be of type int, string given in %s:%d
Stack trace:
#0 %s(%d): SQLite3->__construct(':memory:', 'not-an-int', Object(SensitiveParameterValue))
#1 {main}
TypeError: SQLite3::open(): Argument #2 ($flags) must be of type int, string given in %s:%d
Stack trace:
#0 %s(%d): SQLite3->open(':memory:', 'not-an-int', Object(SensitiveParameterValue))
#1 {main}
