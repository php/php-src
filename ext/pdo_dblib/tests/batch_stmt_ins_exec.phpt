--TEST--
 PDO_DBLIB driver supports a batch of queries containing select, insert, update, exec statements
 this test will fail when using the dblib driver that hasn't been patched
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

// creating a proc need to be a statement in it's own batch, so we need to do a little setup first
$db->query("create table #wf_pdo(id int); ");
$db->query(
"create proc wf_exec_select_proc as " .
"begin " .
"  insert into #wf_pdo values(2), (3), (4); " .
"  select * from #wf_pdo; " .
"end; ");

// now lets get some results
$stmt = $db->query(
"insert into #wf_pdo values(1); " .
"exec wf_exec_select_proc; " .
"drop table #wf_pdo; " .
"drop procedure wf_exec_select_proc; ");

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
