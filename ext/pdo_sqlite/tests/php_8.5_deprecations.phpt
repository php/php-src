--TEST--
PDO_sqlite: PHP 8.5 deprecations
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

var_dump(
     PDO::SQLITE_ATTR_EXTENDED_RESULT_CODES,
     PDO::SQLITE_ATTR_OPEN_FLAGS,
     PDO::SQLITE_ATTR_READONLY_STATEMENT,
     PDO::SQLITE_DETERMINISTIC,
     PDO::SQLITE_OPEN_READONLY,
     PDO::SQLITE_OPEN_READWRITE,
     PDO::SQLITE_OPEN_CREATE,
);

?>
--EXPECTF--
Deprecated: Constant PDO::SQLITE_ATTR_EXTENDED_RESULT_CODES is deprecated since 8.5, use Pdo\Sqlite::ATTR_EXTENDED_RESULT_CODES instead in %s on line %d

Deprecated: Constant PDO::SQLITE_ATTR_OPEN_FLAGS is deprecated since 8.5, use Pdo\Sqlite::ATTR_OPEN_FLAGS instead in %s on line %d

Deprecated: Constant PDO::SQLITE_ATTR_READONLY_STATEMENT is deprecated since 8.5, use Pdo\Sqlite::ATTR_READONLY_STATEMENT instead in %s on line %d

Deprecated: Constant PDO::SQLITE_DETERMINISTIC is deprecated since 8.5, use Pdo\Sqlite::DETERMINISTIC instead in %s on line %d

Deprecated: Constant PDO::SQLITE_OPEN_READONLY is deprecated since 8.5, use Pdo\Sqlite::OPEN_READONLY instead in %s on line %d

Deprecated: Constant PDO::SQLITE_OPEN_READWRITE is deprecated since 8.5, use Pdo\Sqlite::OPEN_READWRITE instead in %s on line %d

Deprecated: Constant PDO::SQLITE_OPEN_CREATE is deprecated since 8.5, use Pdo\Sqlite::OPEN_CREATE instead in %s on line %d
int(1002)
int(1000)
int(1001)
int(2048)
int(1)
int(2)
int(4)
