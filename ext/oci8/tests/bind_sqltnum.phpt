--TEST--
Bind with SQLT_NUM
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
preg_match('/^[[:digit:]]+/', oci_client_version(), $matches);
if (!(isset($matches[0]) && $matches[0] >= 12)) {
    die("skip works only with Oracle 12c or greater version of Oracle client libraries");
}
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table bind_sqltnum_tab",

    "create table bind_sqltnum_tab (
        id                number,
        varchar2_t10      varchar2(10),
        number_t          number,
        number_t92        number(9,2))"
);

oci8_test_sql_execute($c, $stmtarray);

function check_col($c, $colname, $id)
{
    $s = oci_parse($c, "select dump($colname) from bind_sqltnum_tab where id = :id");
    oci_bind_by_name($s, ":id", $id);
    oci_execute($s);
    oci_fetch_all($s, $r);
    var_dump($r);
}


// Run Test

echo "Test 1 - baseline test\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, varchar2_t10) VALUES (100, :c2)");
$c2 = "Hood";
$r = oci_bind_by_name($s, ":c2", $c2, -1);
if (!$r) {
    $e = oci_error($s);
    var_dump($e);
}
$r = oci_execute($s, OCI_DEFAULT);
if (!$r) {
    $e = oci_error($s);
    var_dump($e);
}

$s = oci_parse($c, "select id, varchar2_t10 from bind_sqltnum_tab");
oci_execute($s);
oci_fetch_all($s, $data);
var_dump($data);

echo "Test 2 - SQLT_NUM to a VARCHAR2 column\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, varchar2_t10) VALUES (100, :c2)");
$c2 = "Hood";
$r = oci_bind_by_name($s, ":c2", $c2, -1, SQLT_NUM);
if (!$r) {
    $e = oci_error($s);
    var_dump($e['message']);
}
$r = oci_execute($s, OCI_DEFAULT);
if (!$r) {
    $e = oci_error($s);
    var_dump($e['message']);
}

echo "\nTEST41 wrong bind type SQLT_NUM\n";

$c2 = "Hood41";
$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, varchar2_t10) VALUES (41, :c2)");
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_NUM);
oci_execute($s);

echo "\nTEST42 insert numbers SQLT_NUM\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, number_t) VALUES (42, :n1)");
$n1 = 42;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_NUM);
oci_execute($s);

check_col($c, 'number_t', 42);

echo "\nTEST43 insert numbers SQLT_NUM\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, number_t) VALUES (43, :n1)");
$n1 = 42.69;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_NUM);
oci_execute($s);

check_col($c, 'number_t', 43);

echo "\nTEST44\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, number_t) VALUES (44, :n1)");
$n1 = 0;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_NUM);
oci_execute($s);

check_col($c, 'number_t', 44);

echo "\nTEST45\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, number_t) VALUES (45, :n1)");
$n1 = -23;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_NUM);
oci_execute($s);

check_col($c, 'number_t', 45);

echo "\nTEST46 insert numbers\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, number_t) VALUES (46, :n1)");
$n1 = "-23";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_NUM);
oci_execute($s);

check_col($c, 'number_t', 46);

echo "\nTEST47\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, number_t) VALUES (47, :n1)");
$n1 = "23";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_NUM);
oci_execute($s);

check_col($c, 'number_t', 47);

echo "\nTEST48\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, number_t92) VALUES (48, :n1)");
$n1 = 123.56;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_NUM);
oci_execute($s);

check_col($c, 'number_t92', 48);

echo "\nTEST49\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, number_t92) VALUES (49, :n1)");
$n1 = "123.56";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_NUM);
oci_execute($s);

check_col($c, 'number_t92', 49);

echo "\nTEST50\n";

$s = oci_parse($c, "INSERT INTO bind_sqltnum_tab (id, number_t92) VALUES (50, :n1)");
$n1 = "";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_NUM);
oci_execute($s);

check_col($c, 'number_t92', 50);

// Clean up

$stmtarray = array(
	"drop table bind_sqltnum_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1 - baseline test
array(2) {
  ["ID"]=>
  array(1) {
    [0]=>
    string(3) "100"
  }
  ["VARCHAR2_T10"]=>
  array(1) {
    [0]=>
    string(4) "Hood"
  }
}
Test 2 - SQLT_NUM to a VARCHAR2 column

Warning: oci_execute(): ORA-12899: %s (%s: 40, %s: 10) in %sbind_sqltnum.php on line %d
string(%d) "ORA-12899: %s"

TEST41 wrong bind type SQLT_NUM

Warning: oci_execute(): ORA-12899: %s "%s"."BIND_SQLTNUM_TAB"."VARCHAR2_T10" (%s: 40, %s: 10) in %sbind_sqltnum.php on line %d

TEST42 insert numbers SQLT_NUM
array(1) {
  ["DUMP(NUMBER_T)"]=>
  array(1) {
    [0]=>
    string(29) "Typ=2 Len=8: 42,0,0,0,0,0,0,0"
  }
}

TEST43 insert numbers SQLT_NUM
array(1) {
  ["DUMP(NUMBER_T)"]=>
  array(1) {
    [0]=>
    string(29) "Typ=2 Len=8: 42,0,0,0,0,0,0,0"
  }
}

TEST44
array(1) {
  ["DUMP(NUMBER_T)"]=>
  array(1) {
    [0]=>
    string(28) "Typ=2 Len=8: 0,0,0,0,0,0,0,0"
  }
}

TEST45
array(1) {
  ["DUMP(NUMBER_T)"]=>
  array(1) {
    [0]=>
    string(44) "Typ=2 Len=8: 233,255,255,255,255,255,255,255"
  }
}

TEST46 insert numbers
array(1) {
  ["DUMP(NUMBER_T)"]=>
  array(1) {
    [0]=>
    string(44) "Typ=2 Len=8: 233,255,255,255,255,255,255,255"
  }
}

TEST47
array(1) {
  ["DUMP(NUMBER_T)"]=>
  array(1) {
    [0]=>
    string(29) "Typ=2 Len=8: 23,0,0,0,0,0,0,0"
  }
}

TEST48
array(1) {
  ["DUMP(NUMBER_T92)"]=>
  array(1) {
    [0]=>
    string(16) "Typ=2 Len=1: 128"
  }
}

TEST49
array(1) {
  ["DUMP(NUMBER_T92)"]=>
  array(1) {
    [0]=>
    string(16) "Typ=2 Len=1: 128"
  }
}

TEST50

Warning: oci_execute(): ORA-01438: %s in %sbind_sqltnum.php on line %d
array(1) {
  ["DUMP(NUMBER_T92)"]=>
  array(0) {
  }
}
===DONE===
