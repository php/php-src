--TEST--
 PDO_DBLIB driver supports set rowcount and select @@rowcount in batch statements
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

$stmt = $db->query(
"create table #php_pdo(id int); " .
"set rowcount 2; " .
"insert into #php_pdo values(1), (2), (3); " .
"insert into #php_pdo values(4), (5), (6); " .
"update #php_pdo set id = 4; " .
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
"select * from #php_pdo;" .
"delete from #php_pdo;" .
"drop table #php_pdo;"
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
--XFAIL--
 this test will fail when using the dblib driver that hasn't been patched
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
