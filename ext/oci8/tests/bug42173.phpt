--TEST--
Bug #42173 (TIMESTAMP and INTERVAL query and field functions)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

$stmts = array(

"drop table ts_test",

"create table ts_test (
c1 TIMESTAMP,
c2 TIMESTAMP (5),
c3 TIMESTAMP WITH TIME ZONE,
c4 TIMESTAMP (2) WITH TIME ZONE,
c5 TIMESTAMP WITH LOCAL TIME ZONE,
c6 INTERVAL YEAR TO MONTH,
c7 INTERVAL YEAR(2) TO MONTH,
c8 INTERVAL DAY TO SECOND,
c9 INTERVAL DAY(2) TO SECOND(3)
)",

"insert into ts_test values (
timestamp'1999-01-03 10:00:00.123',
timestamp'1999-01-04 10:00:00.123456',
timestamp'1999-01-05 10:00:00.123456+1:0',
timestamp'1999-01-06 10:00:00.123456-1:0',
timestamp'1999-01-06 10:00:00.123456-1:0',
interval'1-2' year to month,
interval'10-4' year to month,
interval'1 2:20:20.123' day to second,
interval'1 2:20:20.12345' day to second)");

foreach ($stmts as $sql) {
	$s = oci_parse($c, $sql);
	$r = @oci_execute($s);
}

$s = oci_parse($c, "select * from ts_test");
$r = oci_execute($s);
$row = oci_fetch_array($s, OCI_ASSOC);
var_dump($row);

foreach ($row as $name => $field) {
	echo "\nColumn $name\n";
	var_dump(oci_field_is_null($s, $name));
	var_dump(oci_field_name($s, $name));
	var_dump(oci_field_type($s, $name));
	var_dump(oci_field_type_raw($s, $name));
	var_dump(oci_field_scale($s, $name));
	var_dump(oci_field_precision($s, $name));
	var_dump(oci_field_size($s, $name));
}

// Cleanup

$s = oci_parse($c, "drop table ts_test");
$r = @oci_execute($s);

echo "Done\n";

?>
--EXPECTF--
array(9) {
  ["C1"]=>
  string(28) "03-JAN-99 10.00.00.123000 AM"
  ["C2"]=>
  string(27) "04-JAN-99 10.00.00.12346 AM"
  ["C3"]=>
  string(35) "05-JAN-99 10.00.00.123456 AM +01:00"
  ["C4"]=>
  string(31) "06-JAN-99 10.00.00.12 AM -01:00"
  ["C5"]=>
  string(28) "%s"
  ["C6"]=>
  string(6) "+01-02"
  ["C7"]=>
  string(6) "+10-04"
  ["C8"]=>
  string(19) "+01 02:20:20.123000"
  ["C9"]=>
  string(16) "+01 02:20:20.123"
}

Column C1
bool(false)
string(2) "C1"
string(9) "TIMESTAMP"
int(187)
int(6)
int(0)
int(11)

Column C2
bool(false)
string(2) "C2"
string(9) "TIMESTAMP"
int(187)
int(5)
int(0)
int(11)

Column C3
bool(false)
string(2) "C3"
string(23) "TIMESTAMP WITH TIMEZONE"
int(188)
int(6)
int(0)
int(13)

Column C4
bool(false)
string(2) "C4"
string(23) "TIMESTAMP WITH TIMEZONE"
int(188)
int(2)
int(0)
int(13)

Column C5
bool(false)
string(2) "C5"
string(29) "TIMESTAMP WITH LOCAL TIMEZONE"
int(232)
int(6)
int(0)
int(11)

Column C6
bool(false)
string(2) "C6"
string(22) "INTERVAL YEAR TO MONTH"
int(189)
int(0)
int(2)
int(5)

Column C7
bool(false)
string(2) "C7"
string(22) "INTERVAL YEAR TO MONTH"
int(189)
int(0)
int(2)
int(5)

Column C8
bool(false)
string(2) "C8"
string(22) "INTERVAL DAY TO SECOND"
int(190)
int(6)
int(2)
int(11)

Column C9
bool(false)
string(2) "C9"
string(22) "INTERVAL DAY TO SECOND"
int(190)
int(3)
int(2)
int(11)
Done
