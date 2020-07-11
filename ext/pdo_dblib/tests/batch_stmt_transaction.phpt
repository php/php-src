--TEST--
PDO_DBLIB: driver supports a batch of queries containing SELECT, INSERT, UPDATE statements
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';

if (!driver_supports_batch_statements_without_select($db)) die('xfail test will fail with this version of FreeTDS');
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$stmt = $db->query(
"create table #php_pdo(id int);" .
"insert into #php_pdo values(1), (2), (3);" .
"select * from #php_pdo;" .
"begin transaction;" .
"update #php_pdo set id = 4;" .
"rollback transaction;" .
"select * from #php_pdo;" .
"delete from #php_pdo;" .
"drop table #php_pdo;"
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
