--TEST--
PDO_DBLIB: driver supports SET ROWCOUNT and SELECT @@ROWCOUNT in batch statements
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
$stmt = $db->query(
"create table #test_batch_stmt_rowcount(id int); " .
"set rowcount 2; " .
"insert into #test_batch_stmt_rowcount values(1), (2), (3); " .
"insert into #test_batch_stmt_rowcount values(4), (5), (6); " .
"update #test_batch_stmt_rowcount set id = 4; " .
"select @@rowcount; "
);

// check results from the create table
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the set rowcount
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the first insert
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the second insert
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the update
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the select rowcount
var_dump($stmt->fetchAll());
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check that there are no more results
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// now cleanup and check that the results are expected
$stmt = $db->query("set rowcount 0;" .
"select * from #test_batch_stmt_rowcount;" .
"delete from #test_batch_stmt_rowcount;" .
"drop table #test_batch_stmt_rowcount;"
);

// check results from set rowcount
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

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
int(-1)
bool(true)
int(2)
bool(true)
int(2)
bool(true)
int(2)
bool(true)
array(1) {
  [0]=>
  array(2) {
    ["computed"]=>
    int(2)
    [0]=>
    int(2)
  }
}
int(-1)
bool(false)
int(-1)
bool(false)
int(-1)
bool(true)
array(4) {
  [0]=>
  array(2) {
    ["id"]=>
    int(4)
    [0]=>
    int(4)
  }
  [1]=>
  array(2) {
    ["id"]=>
    int(4)
    [0]=>
    int(4)
  }
  [2]=>
  array(2) {
    ["id"]=>
    int(4)
    [0]=>
    int(4)
  }
  [3]=>
  array(2) {
    ["id"]=>
    int(5)
    [0]=>
    int(5)
  }
}
int(-1)
bool(true)
int(4)
bool(true)
int(-1)
bool(false)
int(-1)
bool(false)
