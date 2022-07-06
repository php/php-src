--TEST--
PDO_DBLIB: driver supports a batch of queries containing SELECT, INSERT, UPDATE, EXEC statements
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';

if (!driver_supports_batch_statements_without_select($db)) die('xfail test will fail with this version of FreeTDS');
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

// creating a proc need to be a statement in it's own batch, so we need to do a little setup first
$db->query("create table #php_pdo(id int); ");
$db->query(
"create proc php_pdo_exec_select_proc as " .
"begin " .
"  insert into #php_pdo values(2), (3), (4); " .
"  select * from #php_pdo; " .
"end; "
);

// now lets get some results
$stmt = $db->query(
"insert into #php_pdo values(1); " .
"exec php_pdo_exec_select_proc; " .
"drop table #php_pdo; " .
"drop procedure php_pdo_exec_select_proc; ");

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
