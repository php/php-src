--TEST--
PDO_DBLIB: driver supports multiple queries in a single \PDO::query() call that doesn't return any rowsets
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';

if (!driver_supports_batch_statements_without_select($db)) die('xfail test will fail with this version of FreeTDS');
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$stmt = $db->query(
"create table #test_batch_stmt_ins_up(id int);" .
"insert into #test_batch_stmt_ins_up values(1), (2), (3);" .
"update #test_batch_stmt_ins_up set id = 1;" .
"insert into #test_batch_stmt_ins_up values(2);" .
"drop table #test_batch_stmt_ins_up;"
);

// check results from the create table
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the first insert
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the update
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the second insert
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the drop
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check that there are no more results
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

?>
--EXPECT--
int(-1)
bool(true)
int(3)
bool(true)
int(3)
bool(true)
int(1)
bool(true)
int(-1)
bool(false)
int(-1)
bool(false)
