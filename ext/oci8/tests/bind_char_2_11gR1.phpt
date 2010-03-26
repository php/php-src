--TEST--
SELECT oci_bind_by_name with SQLT_AFC aka CHAR and dates
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
require(dirname(__FILE__)."/connect.inc");
$sv = oci_server_version($c);
$sv = preg_match('/Release 11\.1\./', $sv, $matches);
if ($sv !== 1) {
	die ("skip expected output only valid when using Oracle 11gR1 database");
}
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"alter session set nls_date_format='YYYY-MM-DD'",
	"drop table bind_char_tab",
	"create table bind_char_tab (id number, c1 date)",
	"insert into bind_char_tab values (1, '2008-04-20')",
);
						 
foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	@oci_execute($s);
}

// Run Test

$bv1 = '2008-04-20';

echo "Test 1.1: Type: default.  Length: default\n";
$s = oci_parse($c, "select * from bind_char_tab where c1 = :bv");
$r = oci_bind_by_name($s, ":bv", $bv1);
if ($r)
   do_e_q($s);

echo "Test 1.2: Type: AFC.  Length: default\n";
$r = oci_bind_by_name($s, ":bv", $bv1, -1, SQLT_AFC);
if ($r)
   	do_e_q($s);

echo "Test 1.3: Type: AFC:  Length: 0\n";
$r = oci_bind_by_name($s, ":bv", $bv1, 0, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 1.4: Type: AFC:  Length: strlen\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1), SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 1.5: Type: AFC.  Length: strlen-1\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1)-1, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 1.6: Type: AFC.  Length: strlen+1\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1)+1, SQLT_AFC);
if ($r)
	do_e_q($s);


function do_e_q($s)
{
	echo "  Querying:\n";

	$r = @oci_execute($s);
	if (!$r) {
		$m = oci_error($s);
		echo "    Oci_execute error ORA-".$m['code']." Exiting Query\n";
		return;
	}
	while ($row = oci_fetch_array($s, OCI_ASSOC+OCI_RETURN_NULLS)) {
		foreach ($row as $item) {
			echo "    :" . $item . ":\n";
		}
	}
}

// Cleanup
$stmtarray = array(
	"drop table bind_char_tab"
);
						 
foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

oci_close($c);

echo "Done\n";

?>
--EXPECT--
Test 1.1: Type: default.  Length: default
  Querying:
    :1:
    :2008-04-20:
Test 1.2: Type: AFC.  Length: default
  Querying:
    :1:
    :2008-04-20:
Test 1.3: Type: AFC:  Length: 0
  Querying:
    Oci_execute error ORA-1460 Exiting Query
Test 1.4: Type: AFC:  Length: strlen
  Querying:
    :1:
    :2008-04-20:
Test 1.5: Type: AFC.  Length: strlen-1
  Querying:
    Oci_execute error ORA-1460 Exiting Query
Test 1.6: Type: AFC.  Length: strlen+1
  Querying:
    :1:
    :2008-04-20:
Done
