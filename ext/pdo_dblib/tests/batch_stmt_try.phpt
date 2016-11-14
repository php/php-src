--TEST--
 PDO_DBLIB driver supports using sql servers exceptions
 this test will fail when using the dblib driver that hasn't been patched
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

$stmt = $db->query(
"create table #wf_pdo(id int);" .
"insert into #wf_pdo values(1), (2), (3);" .
"select * from #wf_pdo;" .
"begin try " .
"  update #wf_pdo set id = 'f';" .
"end try " .
"begin catch " .
"  throw;" .
"end catch " .
"select * from #wf_pdo;" .
"delete from #wf_pdo;" .
"drop table #wf_pdo;"
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

// check results from try
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the update
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check that the update statement throws an error
try {
  var_dump($stmt->rowCount());
  var_dump($stmt->nextRowset());
} catch (PDOException $e) {
  var_dump($e->getMessage());
}

// once an error is thrown, the batch is terminated.
// there should no results from here on
// check results from the select
var_dump($stmt->fetchAll());
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the delete
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
int(-1)
bool(true)
int(0)
bool(true)
int(-1)
string(68) "SQLSTATE[HY000]: General error: PDO_DBLIB: dbresults() returned FAIL"
array(0) {
}
int(-1)
bool(false)
int(-1)
bool(false)
int(-1)
bool(false)
int(-1)
bool(false)
