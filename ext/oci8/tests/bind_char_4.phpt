--TEST--
PL/SQL oci_bind_by_name with SQLT_AFC aka CHAR to VARCHAR2 parameter
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

// Same test as bind_char_3 but the PL/SQL function uses VARCHAR2 instead of CHAR

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "create or replace function bind_char_3_fn(p1 varchar2) return varchar2 as begin return p1; end;",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1.1 In Length: default.  In Type: default.  Out Length: default.          Out Type: default\n";

$s = oci_parse($c, "begin :bv2 := bind_char_3_fn(:bv1); end;");
$bv1 = 'abc';
$r = oci_bind_by_name($s, ':bv1', $bv1) && oci_bind_by_name($s, ':bv2', $bv2);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);

echo "Test 1.2 In Length: default.  In Type: default.  Out Length: 10.               Out Type: default\n";

$bv1 = 'abc';
$r = oci_bind_by_name($s, ':bv1', $bv1) && oci_bind_by_name($s, ':bv2', $bv2, 10);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);


echo "Test 1.3 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: default\n";

$bv1 = 'abc';
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 10);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);



echo "Test 1.4 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: AFC\n";

$bv1 = 'abc';
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 10, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);


echo "Test 1.5 In Length: strlen.   In Type: AFC.      Out Length: strlen(input).    Out Type: AFC\n";

$bv1 = 'abc';
$r = oci_bind_by_name($s, ':bv1', $bv1, strlen($bv1), SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, strlen($bv1), SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);


echo "Test 1.6 In Length: strlen.   In Type: AFC.      Out Length: strlen(input)-1.  Out Type: AFC\n";

$bv1 = 'abc';
$r = oci_bind_by_name($s, ':bv1', $bv1, strlen($bv1), SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, strlen($bv1)-1, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);


echo "Test 1.7 In Length: strlen.   In Type: AFC.      Out Length: strlen(input)+1.  Out Type: AFC\n";

$bv1 = 'abc';
$r = oci_bind_by_name($s, ':bv1', $bv1, strlen($bv1), SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, strlen($bv1)+1, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);


echo "\n\nTests with ''\n\n";

echo "Test 2.1 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: AFC\n";

$bv1 = '';
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 10, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);


echo "Test 2.2 In Length: default.  In Type: default.  Out Length: 10.               Out Type: default\n";

$r = oci_bind_by_name($s, ':bv1', $bv1) && oci_bind_by_name($s, ':bv2', $bv2, 10);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);



echo "Test 2.3 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: default\n";

$bv1 = '';
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 10);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);



echo "Test 2.4 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: AFC\n";

$bv1 = '';
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 10, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);



echo "Test 2.5 In Length: -1.       In Type: AFC.      Out Length: 0.                Out Type: AFC\n";

$bv1 = '';
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 0, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);



echo "Test 2.6 In Length: 0.        In Type: AFC.      Out Length: 0.                Out Type: AFC\n";

$bv1 = '';
$r = oci_bind_by_name($s, ':bv1', $bv1, 0, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 0, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);



echo "Test 2.7 In Length: 1.        In Type: AFC.      Out Length: 1.                Out Type: AFC\n";

$bv1 = '';
$r = oci_bind_by_name($s, ':bv1', $bv1, 1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 1, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);



echo "\n\nTests with NULL\n";

echo "Test 3.1 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: AFC\n";

$bv1 = null;
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 10, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);


echo "Test 3.2 In Length: default.  In Type: default.  Out Length: 10.               Out Type: default\n";

$bv1 = null;
$r = oci_bind_by_name($s, ':bv1', $bv1) && oci_bind_by_name($s, ':bv2', $bv2, 10);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);



echo "Test 3.3 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: default\n";

$bv1 = null;
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 10);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);



echo "Test 3.4 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: AFC\n";

$bv1 = null;
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 10, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);


echo "Test 3.5 In Length: -1.       In Type: AFC.      Out Length: 0.                Out Type: AFC\n";

$bv1 = null;
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 0, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);



echo "Test 3.6 In Length: -1.       In Type: AFC.      Out Length: 1.                Out Type: AFC\n";

$bv1 = null;
$r = oci_bind_by_name($s, ':bv1', $bv1, -1, SQLT_AFC) && oci_bind_by_name($s, ':bv2', $bv2, 1, SQLT_AFC);
if ($r)
    do_e($s);
var_dump($bv1, $bv2);


function do_e($s)
{
    echo "  Executing:\n";

    $r = @oci_execute($s);
    if (!$r) {
        $m = oci_error($s);
        echo "    Oci_execute error ORA-".$m['code']."\n";
        return;
    }
}

// Cleanup

$stmtarray = array(
    "drop function bind_char_3_fn"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
Test 1.1 In Length: default.  In Type: default.  Out Length: default.          Out Type: default
  Executing:
    Oci_execute error ORA-6502
string(3) "abc"
NULL
Test 1.2 In Length: default.  In Type: default.  Out Length: 10.               Out Type: default
  Executing:
string(3) "abc"
string(3) "abc"
Test 1.3 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: default
  Executing:
string(3) "abc"
string(3) "abc"
Test 1.4 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: AFC
  Executing:
string(3) "abc"
string(10) "abc       "
Test 1.5 In Length: strlen.   In Type: AFC.      Out Length: strlen(input).    Out Type: AFC
  Executing:
string(3) "abc"
string(3) "abc"
Test 1.6 In Length: strlen.   In Type: AFC.      Out Length: strlen(input)-1.  Out Type: AFC
  Executing:
    Oci_execute error ORA-6502
string(3) "abc"
string(3) "abc"
Test 1.7 In Length: strlen.   In Type: AFC.      Out Length: strlen(input)+1.  Out Type: AFC
  Executing:
string(3) "abc"
string(4) "abc "


Tests with ''

Test 2.1 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: AFC
  Executing:
string(0) ""
NULL
Test 2.2 In Length: default.  In Type: default.  Out Length: 10.               Out Type: default
  Executing:
string(0) ""
NULL
Test 2.3 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: default
  Executing:
string(0) ""
NULL
Test 2.4 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: AFC
  Executing:
string(0) ""
NULL
Test 2.5 In Length: -1.       In Type: AFC.      Out Length: 0.                Out Type: AFC
  Executing:
string(0) ""
NULL
Test 2.6 In Length: 0.        In Type: AFC.      Out Length: 0.                Out Type: AFC
  Executing:
string(0) ""
NULL
Test 2.7 In Length: 1.        In Type: AFC.      Out Length: 1.                Out Type: AFC
  Executing:
string(0) ""
NULL


Tests with NULL
Test 3.1 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: AFC
  Executing:
NULL
NULL
Test 3.2 In Length: default.  In Type: default.  Out Length: 10.               Out Type: default
  Executing:
NULL
NULL
Test 3.3 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: default
  Executing:
NULL
NULL
Test 3.4 In Length: -1.       In Type: AFC.      Out Length: 10.               Out Type: AFC
  Executing:
NULL
NULL
Test 3.5 In Length: -1.       In Type: AFC.      Out Length: 0.                Out Type: AFC
  Executing:
NULL
NULL
Test 3.6 In Length: -1.       In Type: AFC.      Out Length: 1.                Out Type: AFC
  Executing:
NULL
NULL
Done
