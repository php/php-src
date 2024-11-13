--TEST--
PDO_sqlite: Testing constants exist
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

echo "Hello\n";
var_dump(Pdo\Sqlite::DETERMINISTIC);
var_dump(Pdo\Sqlite::ATTR_OPEN_FLAGS);
var_dump(Pdo\Sqlite::OPEN_READONLY);
var_dump(Pdo\Sqlite::OPEN_READWRITE);
var_dump(Pdo\Sqlite::OPEN_CREATE);
var_dump(Pdo\Sqlite::ATTR_READONLY_STATEMENT);
var_dump(Pdo\Sqlite::ATTR_EXTENDED_RESULT_CODES);

?>
--EXPECTF--
Hello
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
