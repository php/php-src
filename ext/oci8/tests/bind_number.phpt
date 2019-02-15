--TEST--
Bind with NUMBER column variants
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
?>
--INI--
precision = 14
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table bind_number_tab",
	"create table bind_number_tab (
					id				  number,
					number_t6		  number(6),
					float_t			  float,
					binary_float_t	  binary_float,
					binary_double_t	  binary_double,
					decimal_t		  decimal,
					integer_t		  integer)"
);

oci8_test_sql_execute($c, $stmtarray);

function check_col($c, $colname, $id)
{
	$s = oci_parse($c, "select $colname from bind_number_tab where id = :id");
	oci_bind_by_name($s, ":id", $id);
	oci_execute($s);
	oci_fetch_all($s, $r);
	var_dump($r);
}

// Run Test

echo "Test 1 - invalid number\n";

$s = oci_parse($c, "INSERT INTO bind_number_tab (id, number_t6) VALUES (1, :n1)");
$n1 = "Hello";
oci_bind_by_name($s, ":n1", $n1);
oci_execute($s);

check_col($c, "number_t6", 1);

echo "\nTEST66 insert a float\n";

$s = oci_parse($c, "INSERT INTO bind_number_tab (id, float_t) VALUES (66, :f1)");
$f1 = 123.456;
oci_bind_by_name($s, ":f1", $f1);
oci_execute($s);

check_col($c, 'float_t', 66);

echo "\nTEST67 insert a binary float\n";

$s = oci_parse($c, "INSERT INTO bind_number_tab (id, binary_float_t) VALUES (67, :f1)");
$f1 = 567.456;
oci_bind_by_name($s, ":f1", $f1);
oci_execute($s);

check_col($c, 'binary_float_t', 67);

echo "\nTEST69 insert a binary double\n";

$s = oci_parse($c, "INSERT INTO bind_number_tab (id, binary_double_t) VALUES (69, :f1)");
$f1 = 567.456;
oci_bind_by_name($s, ":f1", $f1);
oci_execute($s);

check_col($c, 'binary_double_t', 69);

echo "\nTEST71 insert a decimal\n";

$s = oci_parse($c, "INSERT INTO bind_number_tab (id, decimal_t) VALUES (71, :f1)");
$f1 = 123.789;
oci_bind_by_name($s, ":f1", $f1);
oci_execute($s);

check_col($c, 'decimal_t', 71);

echo "\nTEST72 insert a decimal\n";

$s = oci_parse($c, "INSERT INTO bind_number_tab (id, decimal_t) VALUES (72, :f1)");
$f1 = 123.789;
oci_bind_by_name($s, ":f1", $f1, -1, SQLT_NUM);
oci_execute($s);

check_col($c, 'decimal_t', 72);

echo "\nTEST73 insert a double\n";

$s = oci_parse($c, "INSERT INTO bind_number_tab (id, binary_double_t) VALUES (73, :f1)");
$f1 = 483.589;
oci_bind_by_name($s, ":f1", $f1);
oci_execute($s);

check_col($c, 'binary_double_t', 73);

echo "\nTEST75 insert a INTEGER\n";

$s = oci_parse($c, "INSERT INTO bind_number_tab (id, integer_t) VALUES (75, :f1)");
$f1 = 589;
oci_bind_by_name($s, ":f1", $f1);
oci_execute($s);

check_col($c, 'integer_t', 75);

echo "\nTEST76 insert a INTEGER\n";

$s = oci_parse($c, "INSERT INTO bind_number_tab (id, integer_t) VALUES (76, :f1)");
$f1 = 42;
oci_bind_by_name($s, ":f1", $f1, -1, SQLT_INT);
oci_execute($s);

check_col($c, 'integer_t', 76);


// Clean up

$stmtarray = array(
	"drop table bind_number_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1 - invalid number

Warning: oci_execute(): ORA-01722: %s in %sbind_number.php on line %d
array(1) {
  ["NUMBER_T6"]=>
  array(0) {
  }
}

TEST66 insert a float
array(1) {
  ["FLOAT_T"]=>
  array(1) {
    [0]=>
    string(7) "123.456"
  }
}

TEST67 insert a binary float
array(1) {
  ["BINARY_FLOAT_T"]=>
  array(1) {
    [0]=>
    string(%r15|8%r) "%r(5.67455994E\+002|567.4560)%r"
  }
}

TEST69 insert a binary double
array(1) {
  ["BINARY_DOUBLE_T"]=>
  array(1) {
    [0]=>
    string(%r23|16%r) "%r(5.6745600000000002E\+002|567.456000000000)%r"
  }
}

TEST71 insert a decimal
array(1) {
  ["DECIMAL_T"]=>
  array(1) {
    [0]=>
    string(3) "124"
  }
}

TEST72 insert a decimal
array(1) {
  ["DECIMAL_T"]=>
  array(1) {
    [0]=>
    string(1) "0"
  }
}

TEST73 insert a double
array(1) {
  ["BINARY_DOUBLE_T"]=>
  array(1) {
    [0]=>
    string(%r12|16%r) "%r(4.83589E\+002|483.589000000000)%r"
  }
}

TEST75 insert a INTEGER
array(1) {
  ["INTEGER_T"]=>
  array(1) {
    [0]=>
    string(3) "589"
  }
}

TEST76 insert a INTEGER
array(1) {
  ["INTEGER_T"]=>
  array(1) {
    [0]=>
    string(2) "42"
  }
}
===DONE===
