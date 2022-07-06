--TEST--
SELECT oci_bind_by_name with SQLT_AFC aka CHAR and dates
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
require(__DIR__."/connect.inc");
// The bind buffer size edge cases seem to change each DB version.
preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] >= 12)) {
    die("skip expected output only valid when using Oracle 12c database");
}
?>
--ENV--
NLS_LANG=.AL32UTF8
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "alter session set nls_date_format='YYYY-MM-DD'",
    "drop table bind_char_tab",
    "create table bind_char_tab (id number, c1 date)",
    "insert into bind_char_tab values (1, '2008-04-20')",
);

oci8_test_sql_execute($c, $stmtarray);

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

oci8_test_sql_execute($c, $stmtarray);

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
    :1:
    :2008-04-20:
Test 1.4: Type: AFC:  Length: strlen
  Querying:
    :1:
    :2008-04-20:
Test 1.5: Type: AFC.  Length: strlen-1
  Querying:
Test 1.6: Type: AFC.  Length: strlen+1
  Querying:
    :1:
    :2008-04-20:
Done
