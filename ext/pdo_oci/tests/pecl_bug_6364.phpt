--TEST--
PECL PDO_OCI Bug #6364 (segmentation fault on stored procedure call with OUT binds)
--EXTENSIONS--
pdo
pdo_oci
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('xleak leaks memory under asan');
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require __DIR__ . '/../../pdo/tests/pdo_test.inc';
$dbh = PDOTest::factory();

$dbh->exec ("create table test6364 (c1 varchar2(10), c2 varchar2(10), c3 varchar2(10), c4 varchar2(10), c5 varchar2(10))");

$dbh->exec ("create procedure test6364_sp(p1 IN varchar2, p2 IN varchar2, p3 IN varchar2, p4 OUT varchar2, p5 OUT varchar2) as begin insert into test6364 (c1, c2, c3) values (p1, p2, p3); p4 := 'val4'; p5 := 'val5'; end;");

$stmt = $dbh->prepare("call test6364_sp('p1','p2','p3',?,?)");

$out_param1 = "a";
$out_param2 = "a";

$stmt->bindParam(1, $out_param1,PDO::PARAM_STR, 1024);
$stmt->bindParam(2, $out_param2,PDO::PARAM_STR, 1024);

$stmt->execute() or die ("Execution error: " . var_dump($dbh->errorInfo()));

var_dump($out_param1);
var_dump($out_param2);

foreach ($dbh->query("select * from test6364") as $row) {
    var_dump($row);
}

print "Done\n";
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');
PDOTest::dropTableIfExists($db, "test6364");
$db->exec("DROP PROCEDURE test6364_sp");
?>
--EXPECT--
string(4) "val4"
string(4) "val5"
array(10) {
  ["c1"]=>
  string(2) "p1"
  [0]=>
  string(2) "p1"
  ["c2"]=>
  string(2) "p2"
  [1]=>
  string(2) "p2"
  ["c3"]=>
  string(2) "p3"
  [2]=>
  string(2) "p3"
  ["c4"]=>
  NULL
  [3]=>
  NULL
  ["c5"]=>
  NULL
  [4]=>
  NULL
}
Done
