--TEST--
SELECT oci_bind_by_name with SQLT_AFC aka CHAR
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

// Output is for 32 bit client to 64bit 11.1.0.6

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table bind_char_tab",
	"create table bind_char_tab (id number, c1 char(10), c2 varchar2(10))",
	"insert into bind_char_tab values (1, 'abc', NULL)",
	"insert into bind_char_tab values (2, NULL, 'abc')",
	"insert into bind_char_tab values (3, NULL, 'abc       ')"
);
						 
foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	@oci_execute($s);
}

// Run Test

echo "*** Non-null Data Tests against CHAR***\n";

$bv1 = 'abc';

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

echo "\n\n*** NULL data tests against CHAR ***\n";

$bv1 = null;

echo "Test 2.1: Type: default.  Length: default\n";
$s = oci_parse($c, "select * from bind_char_tab where c1 = :bv");
$r = oci_bind_by_name($s, ":bv", $bv1);
if ($r)
	do_e_q($s);

echo "Test 2.2: Type: AFC.  Length: default\n";
$r = oci_bind_by_name($s, ":bv", $bv1, -1, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 2.3: Type: AFC:  Length: 0\n";
$r = oci_bind_by_name($s, ":bv", $bv1, 0, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 2.4: Type: AFC:  Length: strlen\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1), SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 2.5: Type: AFC.  Length: strlen-1\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1)-1, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 2.6: Type: AFC.  Length: strlen+1\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1)+1, SQLT_AFC);
if ($r)
	do_e_q($s);


echo "\n\n*** Non-null Data Tests against VARCHAR2***\n";

$bv1 = 'abc';

echo "Test 3.1: Type: default.  Length: default\n";
$s = oci_parse($c, "select * from bind_char_tab where c2 = :bv");
$r = oci_bind_by_name($s, ":bv", $bv1);
if ($r)
	do_e_q($s);

echo "Test 3.2: Type: AFC.  Length: default\n";
$r = oci_bind_by_name($s, ":bv", $bv1, -1, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 3.3: Type: AFC:  Length: 0\n";
$r = oci_bind_by_name($s, ":bv", $bv1, 0, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 3.4: Type: AFC:  Length: strlen\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1), SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 3.5: Type: AFC.  Length: strlen-1\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1)-1, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 3.6: Type: AFC.  Length: strlen+1\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1)+1, SQLT_AFC);
if ($r)
	do_e_q($s);


echo "\n\n*** NULL data tests against VARCHAR2 ***\n";

$bv1 = null;

echo "Test 4.1: Type: default.  Length: default\n";
$s = oci_parse($c, "select * from bind_char_tab where c2 = :bv");
$r = oci_bind_by_name($s, ":bv", $bv1);
if ($r)
	do_e_q($s);

echo "Test 4.2: Type: AFC.  Length: default\n";
$r = oci_bind_by_name($s, ":bv", $bv1, -1, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 4.3: Type: AFC:  Length: 0\n";
$r = oci_bind_by_name($s, ":bv", $bv1, 0, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 4.4: Type: AFC:  Length: strlen\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1), SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 4.5: Type: AFC.  Length: strlen-1\n";
$r = oci_bind_by_name($s, ":bv", $bv1, strlen($bv1)-1, SQLT_AFC);
if ($r)
	do_e_q($s);

echo "Test 4.6: Type: AFC.  Length: strlen+1\n";
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
*** Non-null Data Tests against CHAR***
Test 1.1: Type: default.  Length: default
  Querying:
Test 1.2: Type: AFC.  Length: default
  Querying:
    :1:
    :abc       :
    ::
Test 1.3: Type: AFC:  Length: 0
  Querying:
    Oci_execute error ORA-1460 Exiting Query
Test 1.4: Type: AFC:  Length: strlen
  Querying:
    :1:
    :abc       :
    ::
Test 1.5: Type: AFC.  Length: strlen-1
  Querying:
    Oci_execute error ORA-1460 Exiting Query
Test 1.6: Type: AFC.  Length: strlen+1
  Querying:
    :1:
    :abc       :
    ::


*** NULL data tests against CHAR ***
Test 2.1: Type: default.  Length: default
  Querying:
Test 2.2: Type: AFC.  Length: default
  Querying:
Test 2.3: Type: AFC:  Length: 0
  Querying:
Test 2.4: Type: AFC:  Length: strlen
  Querying:
Test 2.5: Type: AFC.  Length: strlen-1
  Querying:
Test 2.6: Type: AFC.  Length: strlen+1
  Querying:


*** Non-null Data Tests against VARCHAR2***
Test 3.1: Type: default.  Length: default
  Querying:
    :2:
    ::
    :abc:
Test 3.2: Type: AFC.  Length: default
  Querying:
    :2:
    ::
    :abc:
Test 3.3: Type: AFC:  Length: 0
  Querying:
    Oci_execute error ORA-1460 Exiting Query
Test 3.4: Type: AFC:  Length: strlen
  Querying:
    :2:
    ::
    :abc:
Test 3.5: Type: AFC.  Length: strlen-1
  Querying:
    Oci_execute error ORA-1460 Exiting Query
Test 3.6: Type: AFC.  Length: strlen+1
  Querying:
    :2:
    ::
    :abc:


*** NULL data tests against VARCHAR2 ***
Test 4.1: Type: default.  Length: default
  Querying:
Test 4.2: Type: AFC.  Length: default
  Querying:
Test 4.3: Type: AFC:  Length: 0
  Querying:
Test 4.4: Type: AFC:  Length: strlen
  Querying:
Test 4.5: Type: AFC.  Length: strlen-1
  Querying:
Test 4.6: Type: AFC.  Length: strlen+1
  Querying:
Done
