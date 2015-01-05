--TEST--
Bind with various WHERE conditions
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table bind_query_tab",
	"create table bind_query_tab (empno number(4), ename varchar2(10), sal number(7,2))",
	"insert into bind_query_tab values (7934, 'MILLER', 1300)",
	"insert into bind_query_tab values (7902, 'FORD', 3000)"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";

$e = 7934;

$s = oci_parse($c, "select ename from bind_query_tab where empno = :eno");
oci_bind_by_name( $s, ":eno", $e, -1, SQLT_INT);
oci_execute($s);
var_dump(oci_fetch_row($s));

echo "Test 2\n";

$v = 1000;
$s = oci_parse($c, 'select ename from bind_query_tab where sal > :v order by ename');
oci_bind_by_name( $s, ":v", $v);
oci_define_by_name($s, "ENAME", $ename, 20);
oci_execute($s);
while (oci_fetch($s)) {
    var_dump($ename);
}


echo "Test 3\n";

$s = oci_parse($c, 'select ename from bind_query_tab where sal > :v order by ename');
oci_bind_by_name( $s, ":v", $v);
$v = 2000;
oci_define_by_name($s, "ENAME", $ename, 20);
oci_execute($s);
while (oci_fetch($s)) {
    var_dump($ename);
}


// Clean up

$stmtarray = array(
	"drop table bind_query_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
array(1) {
  [0]=>
  string(6) "MILLER"
}
Test 2
string(4) "FORD"
string(6) "MILLER"
Test 3
string(4) "FORD"
===DONE===
