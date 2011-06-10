--TEST--
Bind with SQLT_INT
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table bind_sqltint_tab",

    "create table bind_sqltint_tab (
        id                number,
        varchar2_t10      varchar2(10),
        number_t          number,
        number_t92        number(9,2))",

);

oci8_test_sql_execute($c, $stmtarray);

function check_col($c, $colname, $id)
{
    $s = oci_parse($c, "select $colname from bind_sqltint_tab where id = :id");
    oci_bind_by_name($s, ":id", $id);
    oci_execute($s);
    oci_fetch_all($s, $r);
    var_dump($r);
}

// Run Test

echo "\nTEST141 wrong bind type SQLT_INT\n";

$c2 = "Hood141";
$s = oci_parse($c, "INSERT INTO bind_sqltint_tab (id, varchar2_t10) VALUES (141, :c2)");
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'varchar2_t10', 141);

echo "\nTEST142 insert numbers SQLT_INT\n";

$s = oci_parse($c, "INSERT INTO bind_sqltint_tab (id, number_t) VALUES (142, :n1)");
$n1 = 42;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'number_t', 142);

echo "\nTEST143 insert numbers, SQLT_INT\n";

$s = oci_parse($c, "INSERT INTO bind_sqltint_tab (id, number_t) VALUES (143, :n1)");
$n1 = 42.69;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'number_t', 143);

echo "\nTEST144 insert numbers with SQLT_INT\n";

$s = oci_parse($c, "INSERT INTO bind_sqltint_tab (id, number_t) VALUES (144, :n1)");
$n1 = 0;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'number_t', 144);

echo "\nTEST145 insert numbers with SQLT_INT\n";

$s = oci_parse($c, "INSERT INTO bind_sqltint_tab (id, number_t) VALUES (145, :n1)");
$n1 = -23;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'number_t', 145);

echo "\nTEST146 insert numbers\n";

$s = oci_parse($c, "INSERT INTO bind_sqltint_tab (id, number_t) VALUES (146, :n1)");
$n1 = "-23";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'number_t', 146);

echo "\nTEST147 insert numbers with SQLT_INT\n";

$s = oci_parse($c, "INSERT INTO bind_sqltint_tab (id, number_t) VALUES (147, :n1)");
$n1 = "23";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'number_t', 147);

echo "\nTEST148 insert numbers with SQLT_INT\n";

$s = oci_parse($c, "INSERT INTO bind_sqltint_tab (id, number_t92) VALUES (148, :n1)");
$n1 = 123.56;
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'number_t92', 148);

echo "\nTEST149 insert numbers with SQLT_INT\n";

$s = oci_parse($c, "INSERT INTO bind_sqltint_tab (id, number_t92) VALUES (149, :n1)");
$n1 = "123.56";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'number_t92', 149);

echo "\nTEST150 insert numbers with SQLT_INT\n";

$s = oci_parse($c, "INSERT INTO bind_sqltint_tab (id, number_t92) VALUES (150, :n1)");
$n1 = "";
oci_bind_by_name($s, ":n1", $n1, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'number_t92', 150);

// Clean up

$stmtarray = array(
	"drop table bind_sqltint_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
TEST141 wrong bind type SQLT_INT
array(1) {
  ["VARCHAR2_T10"]=>
  array(1) {
    [0]=>
    string(1) "0"
  }
}

TEST142 insert numbers SQLT_INT
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(2) "42"
  }
}

TEST143 insert numbers, SQLT_INT
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(2) "42"
  }
}

TEST144 insert numbers with SQLT_INT
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(1) "0"
  }
}

TEST145 insert numbers with SQLT_INT
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(3) "-23"
  }
}

TEST146 insert numbers
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(3) "-23"
  }
}

TEST147 insert numbers with SQLT_INT
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(2) "23"
  }
}

TEST148 insert numbers with SQLT_INT
array(1) {
  ["NUMBER_T92"]=>
  array(1) {
    [0]=>
    string(3) "123"
  }
}

TEST149 insert numbers with SQLT_INT
array(1) {
  ["NUMBER_T92"]=>
  array(1) {
    [0]=>
    string(3) "123"
  }
}

TEST150 insert numbers with SQLT_INT
array(1) {
  ["NUMBER_T92"]=>
  array(1) {
    [0]=>
    string(1) "0"
  }
}
===DONE===
