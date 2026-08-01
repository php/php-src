--TEST--
PDO_DBLIB: driver supports a batch of queries containing SELECT, INSERT, UPDATE, EXEC statements
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
$db = getDbConnection();
if (!driver_supports_batch_statements_without_select($db)) die('xfail test will fail with this version of FreeTDS');
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$db = getDbConnection();
// creating a proc need to be a statement in it's own batch, so we need to do a little setup first
$db->query("create table #test_batch_stmt_ins_exec(id int); ");
$db->query(
"create proc test_proc_batch_stmt_ins_exec as " .
"begin " .
"  insert into #test_batch_stmt_ins_exec values(2), (3), (4); " .
"  select * from #test_batch_stmt_ins_exec; " .
"end; "
);

// now lets get some results
$stmt = $db->query(
"insert into #test_batch_stmt_ins_exec values(1); " .
"exec test_proc_batch_stmt_ins_exec; " .
"drop table #test_batch_stmt_ins_exec; " .
"drop procedure test_proc_batch_stmt_ins_exec; ");

// check results from the insert
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the exec
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the drop table
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the drop procedure
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check that there are no more results
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

?>
--EXPECT--
int(1)
bool(true)
int(-1)
bool(true)
int(-1)
bool(true)
int(-1)
bool(false)
int(-1)
bool(false)
