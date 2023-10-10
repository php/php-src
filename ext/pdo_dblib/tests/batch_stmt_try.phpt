--TEST--
PDO_DBLIB: driver supports exceptions
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
"create table #test_batch_stmt_try(id int);" .
"insert into #test_batch_stmt_try values(1), (2), (3);" .
"select * from #test_batch_stmt_try;" .
"begin try " .
"  update #test_batch_stmt_try set id = 'f';" .
"end try " .
"begin catch " .
"  throw;" .
"end catch " .
"select * from #test_batch_stmt_try;" .
"delete from #test_batch_stmt_try;" .
"drop table #test_batch_stmt_try;"
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
