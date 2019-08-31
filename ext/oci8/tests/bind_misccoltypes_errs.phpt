--TEST--
Bind miscellaneous column types and generating errors
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table bind_misccoltypes_errs_tab",

	"create table bind_misccoltypes_errs_tab (
        id                number,
        char_t            char(1),
        char_t10          char(10),
        varchar2_t10      varchar2(10),
        number_t          number,
        number_t92        number(9,2),
        number_t6         number(6),
        date_t            date,
        timestamp_t       timestamp,
        float_t           float,
        binary_float_t    binary_float,
        binary_double_t   binary_double,
        decimal_t         decimal,
        integer_t         integer,
        nchar_t           nchar(10),
        nvarchar2_t10     nvarchar2(10),
        varchar_t10       varchar(10) )",
);

oci8_test_sql_execute($c, $stmtarray);

function check_col($c, $colname, $id)
{
    $s = oci_parse($c, "select $colname from bind_misccoltypes_errs_tab where id = :id");
    oci_bind_by_name($s, ":id", $id);
    oci_execute($s);
    oci_fetch_all($s, $r);
    var_dump($r);
}

// Tests

echo "\nTest 1 insert numbers\n";

$n1 = -23253245.3432467;

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_errs_tab (id, number_t6) VALUES (1, :n1)");
oci_bind_by_name($s, ":n1", $n1);
oci_execute($s);

check_col($c, 'number_t6', 57);

echo "\nTest 2 insert numbers\n";

$n1 = "Hello";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_errs_tab (id, number_t6) VALUES (2, :n1)");
oci_bind_by_name($s, ":n1", $n1);
oci_execute($s);

check_col($c, 'number_t6', 57);

echo "\nTest 3 - too long CHAR\n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_errs_tab (id, char_t) VALUES (3, :c2)");
$c2 = "AB";
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_AFC);
oci_execute($s);

echo "\nTest 4 - too long VARCHAR2\n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_errs_tab (id, varchar2_t10) VALUES (4, :c2)");
$c2 = "AAAAAAAAAAB";
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_AFC);
oci_execute($s);

echo "\nTest 5 - invalid number\n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_errs_tab (id, number_t) VALUES (5, :c2)");
$c2 = "ABC";
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_AFC);
oci_execute($s);

echo "\nTest 6 - insert a VARCHAR2 with SQLT_BIN\n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_errs_tab (id, varchar2_t10) VALUES (6, :c2)");
$c2 = "Hood 6";
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_BIN);
oci_execute($s);

check_col($c, 'varchar2_t10', 6);

echo "\nTest 7 - insert a VARCHAR2 with SQLT_LBI\n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_errs_tab (id, varchar2_t10) VALUES (7, :c2)");
$c2 = "Hood 7";
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_LBI);
oci_execute($s);

check_col($c, 'varchar2_t10', 7);


// Clean up

$stmtarray = array(
	"drop table bind_misccoltypes_errs_tab"
);

oci8_test_sql_execute($c, $stmtarray);

oci_close($c);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1 insert numbers

Warning: oci_execute(): ORA-01438: %s in %sbind_misccoltypes_errs.php on line %d
array(1) {
  ["NUMBER_T6"]=>
  array(0) {
  }
}

Test 2 insert numbers

Warning: oci_execute(): ORA-01722: %s in %sbind_misccoltypes_errs.php on line %d
array(1) {
  ["NUMBER_T6"]=>
  array(0) {
  }
}

Test 3 - too long CHAR

Warning: oci_execute(): ORA-12899: %r(%s "%s"."BIND_MISCCOLTYPES_ERRS_TAB"."CHAR_T" \(%s: 2, %s: 1\)|String data right truncation)%r in %sbind_misccoltypes_errs.php on line %d

Test 4 - too long VARCHAR2

Warning: oci_execute(): ORA-12899: %r(%s "%s"."BIND_MISCCOLTYPES_ERRS_TAB"."VARCHAR2_T10" \(%s: 11, %s: 10\)|%s data right truncation)%r in %sbind_misccoltypes_errs.php on line %d

Test 5 - invalid number

Warning: oci_execute(): ORA-01722: %s in %sbind_misccoltypes_errs.php on line %d

Test 6 - insert a VARCHAR2 with SQLT_BIN

Warning: oci_execute(): ORA-12899: %r(%s "%s"."BIND_MISCCOLTYPES_ERRS_TAB"."VARCHAR2_T10" \(%s: 12, %s: 10\)|String data right truncation)%r in %sbind_misccoltypes_errs.php on line %d
array(1) {
  ["VARCHAR2_T10"]=>
  array(0) {
  }
}

Test 7 - insert a VARCHAR2 with SQLT_LBI

Warning: oci_execute(): ORA-12899: %r(%s "%s"."BIND_MISCCOLTYPES_ERRS_TAB"."VARCHAR2_T10" \(%s: 12, %s: 10\)|String data right truncation)%r in %sbind_misccoltypes_errs.php on line %d
array(1) {
  ["VARCHAR2_T10"]=>
  array(0) {
  }
}
===DONE===
