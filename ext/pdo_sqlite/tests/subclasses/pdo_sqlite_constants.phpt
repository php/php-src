--TEST--
PDO_sqlite: Testing constants exist
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

echo "Hello\n";
var_dump(PdoSqlite::DETERMINISTIC);
var_dump(PdoSqlite::ATTR_OPEN_FLAGS);
var_dump(PdoSqlite::OPEN_READONLY);
var_dump(PdoSqlite::OPEN_READWRITE);
var_dump(PdoSqlite::OPEN_CREATE);
var_dump(PdoSqlite::ATTR_READONLY_STATEMENT);
var_dump(PdoSqlite::ATTR_EXTENDED_RESULT_CODES);

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
