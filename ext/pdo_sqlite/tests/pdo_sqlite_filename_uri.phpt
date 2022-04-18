--TEST--
PDO_sqlite: Testing filename uri
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

// create with in-memory database using shared cached
$db = new PDO('sqlite:file::memory:?cache=shared');

var_dump($db->exec('CREATE TABLE test1 (id INT);'));

// create second connection to in-memory database
$db = new PDO('sqlite:file::memory:?cache=shared');

var_dump($db->exec('SELECT * from test1'));

// create with default read-write|create mode
$filename = "file:" . __DIR__ . DIRECTORY_SEPARATOR . "pdo_sqlite_filename_uri.db";

$db = new PDO('sqlite:' . $filename);

var_dump($db->exec('CREATE TABLE test1 (id INT);'));

// create with readonly mode
$filename = "file:" . __DIR__ . DIRECTORY_SEPARATOR . "pdo_sqlite_filename_uri.db?mode=ro";

$db = new PDO('sqlite:' . $filename);

var_dump($db->exec('CREATE TABLE test2 (id INT);'));

?>
--CLEAN--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . "pdo_sqlite_filename_uri.db";
if (file_exists($filename)) {
    unlink($filename);
}
?>
--EXPECTF--
int(0)
int(0)
int(0)

Fatal error: Uncaught PDOException: SQLSTATE[HY000]: General error: 8 attempt to write a readonly database in %s
Stack trace:
%s
#1 {main}
  thrown in %s
