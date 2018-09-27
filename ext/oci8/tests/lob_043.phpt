--TEST--
Bug #49560 (LOB resource destructor and refcount test)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table lob_043_tab",
	"create table lob_043_tab(id number, c1 clob)",
	"begin
       for i in 1..50000 loop
         insert into lob_043_tab (id, c1) values (i, i || ' abcdefghijklmnopq');
      end loop;
     end;",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

function f1($c)
{
    $s = oci_parse($c, 'select id, c1 from lob_043_tab order by id');
    oci_execute($s);
    $r = array();
    while (($row = oci_fetch_array($s, OCI_RETURN_NULLS+OCI_ASSOC+OCI_RETURN_LOBS)) !== false) {
        $r[] = $row['C1'];
    }
    echo "f1 ended\n";
    return $r;
}

function f2($c)
{
    $s = oci_parse($c, 'select id, c1 from lob_043_tab order by id');
    oci_execute($s);
    $r = array();
    while (($row = oci_fetch_array($s, OCI_RETURN_NULLS+OCI_ASSOC)) !== false) {
        $r[] = $row['C1'];
    }
    echo "f2 ended\n";
    return $r;
}

echo "Test 1\n";
$r = f1($c);
/*
  foreach ($r as $v) {
  echo $v, "\n";
  }
*/

echo "Test 2\n";
$r = f2($c);
/*
  foreach ($r as $v) {
  echo $v->load(), "\n";
  }
*/

// Clean up

$stmtarray = array(
	"drop table lob_043_tab"
);

oci8_test_sql_execute($c, $stmtarray);

oci_close($c);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1
f1 ended
Test 2
f2 ended
===DONE===
