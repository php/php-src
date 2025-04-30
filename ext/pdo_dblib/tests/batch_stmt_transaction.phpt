--TEST--
PDO_DBLIB: driver supports a batch of queries containing SELECT, INSERT, UPDATE statements
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
"create table #test_batch_stmt_transaction(id int);" .
"insert into #test_batch_stmt_transaction values(1), (2), (3);" .
"select * from #test_batch_stmt_transaction;" .
"begin transaction;" .
"update #test_batch_stmt_transaction set id = 4;" .
"rollback transaction;" .
"select * from #test_batch_stmt_transaction;" .
"delete from #test_batch_stmt_transaction;" .
"drop table #test_batch_stmt_transaction;"
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

// check results from begin transaction
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from the update
var_dump($stmt->rowCount());
var_dump($stmt->nextRowset());

// check results from rollback
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
int(3)
bool(true)
int(-1)
bool(true)
int(-1)
bool(true)
int(3)
bool(true)
int(-1)
bool(true)
array(3) {
  [0]=>
  array(2) {
    ["id"]=>
    int(1)
    [0]=>
    int(1)
  }
  [1]=>
  array(2) {
    ["id"]=>
    int(2)
    [0]=>
    int(2)
  }
  [2]=>
  array(2) {
    ["id"]=>
    int(3)
    [0]=>
    int(3)
  }
}
int(-1)
bool(true)
int(3)
bool(true)
int(-1)
bool(false)
int(-1)
bool(false)
