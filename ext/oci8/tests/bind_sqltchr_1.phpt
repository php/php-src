--TEST--
Bind with SQLT_CHR
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

// Initialization

$stmtarray = array(
    "drop table bind_sqltchr_tab",

    "create table bind_sqltchr_tab (
        id                number,
        varchar2_t10      varchar2(10),
        number_t          number,
        number_t92        number(9,2))"

);

oci8_test_sql_execute($c, $stmtarray);

function check_col($c, $colname, $id)
{
    $s = oci_parse($c, "select $colname from bind_sqltchr_tab where id = :id");
    oci_bind_by_name($s, ":id", $id);
    oci_execute($s);
    oci_fetch_all($s, $r);
    var_dump($r);
}

// Run Test

echo "\nTEST241 bind SQLT_CHR\n";

$c2 = "Hood241";
$s = oci_parse($c, "INSERT INTO bind_sqltchr_tab (id, varchar2_t10) VALUES (241, :c2)");
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_CHR);
oci_execute($s);

check_col($c, 'varchar2_t10', 241);


echo "\nTEST242 insert numbers SQLT_CHR\n";

$s = oci_parse($c, "INSERT INTO bind_sqltchr_tab (id, number_t) VALUES (242, :n1)");
$n1 = 42;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_CHR);
oci_execute($s);

check_col($c, 'number_t', 242);

echo "\nTEST243 insert numbers, SQLT_CHR\n";

$s = oci_parse($c, "INSERT INTO bind_sqltchr_tab (id, number_t) VALUES (243, :n1)");
$n1 = 42.69;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_CHR);
oci_execute($s);

check_col($c, 'number_t', 243);

echo "\nTEST244 insert numbers with SQLT_CHR\n";

$s = oci_parse($c, "INSERT INTO bind_sqltchr_tab (id, number_t) VALUES (244, :n1)");
$n1 = 0;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_CHR);
oci_execute($s);

check_col($c, 'number_t', 244);

echo "\nTEST245 insert numbers with SQLT_CHR\n";

$s = oci_parse($c, "INSERT INTO bind_sqltchr_tab (id, number_t) VALUES (245, :n1)");
$n1 = -23;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_CHR);
oci_execute($s);

check_col($c, 'number_t', 245);

echo "\nTEST246 insert numbers\n";

$s = oci_parse($c, "INSERT INTO bind_sqltchr_tab (id, number_t) VALUES (246, :n1)");
$n1 = "-23";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_CHR);
oci_execute($s);

check_col($c, 'number_t', 246);

echo "\nTEST247 insert numbers with SQLT_CHR\n";

$s = oci_parse($c, "INSERT INTO bind_sqltchr_tab (id, number_t) VALUES (247, :n1)");
$n1 = "23";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_CHR);
oci_execute($s);

check_col($c, 'number_t', 247);

echo "\nTEST248 insert numbers with SQLT_CHR\n";

$s = oci_parse($c, "INSERT INTO bind_sqltchr_tab (id, number_t92) VALUES (248, :n1)");
$n1 = 123.56;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_CHR);
oci_execute($s);

check_col($c, 'number_t92', 248);

echo "\nTEST249 insert numbers with SQLT_CHR\n";

$s = oci_parse($c, "INSERT INTO bind_sqltchr_tab (id, number_t92) VALUES (249, :n1)");
$n1 = "123.56";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_CHR);
oci_execute($s);

check_col($c, 'number_t92', 249);

echo "\nTEST250 insert numbers with SQLT_CHR\n";

$s = oci_parse($c, "INSERT INTO bind_sqltchr_tab (id, number_t92) VALUES (250, :n1)");
$n1 = "";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_CHR);
oci_execute($s);

check_col($c, 'number_t92', 250);

// Clean up

$stmtarray = array(
    "drop table bind_sqltchr_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECT--
TEST241 bind SQLT_CHR
array(1) {
  ["VARCHAR2_T10"]=>
  array(1) {
    [0]=>
    string(7) "Hood241"
  }
}

TEST242 insert numbers SQLT_CHR
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(2) "42"
  }
}

TEST243 insert numbers, SQLT_CHR
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(5) "42.69"
  }
}

TEST244 insert numbers with SQLT_CHR
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(1) "0"
  }
}

TEST245 insert numbers with SQLT_CHR
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(3) "-23"
  }
}

TEST246 insert numbers
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(3) "-23"
  }
}

TEST247 insert numbers with SQLT_CHR
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(2) "23"
  }
}

TEST248 insert numbers with SQLT_CHR
array(1) {
  ["NUMBER_T92"]=>
  array(1) {
    [0]=>
    string(6) "123.56"
  }
}

TEST249 insert numbers with SQLT_CHR
array(1) {
  ["NUMBER_T92"]=>
  array(1) {
    [0]=>
    string(6) "123.56"
  }
}

TEST250 insert numbers with SQLT_CHR
array(1) {
  ["NUMBER_T92"]=>
  array(1) {
    [0]=>
    NULL
  }
}
