--TEST--
PDO_DBLIB: driver supports a batch of queries containing SELECT, INSERT, UPDATE statements
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
"create table #test_batch_stmt_ins_sel_up_del(id int);" .
"insert into #test_batch_stmt_ins_sel_up_del values(1), (2), (3);" .
"select * from #test_batch_stmt_ins_sel_up_del;" .
"update #test_batch_stmt_ins_sel_up_del set id = 4;" .
"delete from #test_batch_stmt_ins_sel_up_del;" .
"select * from #test_batch_stmt_ins_sel_up_del;" .
"drop table #test_batch_stmt_ins_sel_up_del;"
);

// check results from the create table
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the first insert
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the select
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the update
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the delete
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the select
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
int(-1)
bool(true)
int(3)
bool(true)
int(3)
bool(true)
int(0)
bool(true)
int(-1)
bool(false)
int(-1)
bool(false)
