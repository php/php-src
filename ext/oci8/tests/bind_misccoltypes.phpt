--TEST--
Bind miscellaneous column types using default types
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(

    "alter session set nls_date_format = 'DD-MON-YY'",

	"drop table bind_misccoltypes_tab",

	"create table bind_misccoltypes_tab (
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
    $s = oci_parse($c, "select $colname from bind_misccoltypes_tab where id = :id");
    oci_bind_by_name($s, ":id", $id);
    oci_execute($s);
    oci_fetch_all($s, $r);
    var_dump($r);
}

// Tests

echo "\nTEST86 insert all ORATYPE values\n";

$insert_sql = "INSERT INTO bind_misccoltypes_tab ( id, "
    . "   char_t, "
    . "   char_t10, "
    . "   varchar2_t10, "
    . "   number_t, "
    . "   number_t92, "
    . "   number_t6, "
    . "   date_t, "
    . "   timestamp_t, "
    . "   float_t, "
    . "   binary_float_t, "
    . "   binary_double_t, "
    . "   decimal_t, "
    . "   integer_t, "
    . "   nchar_t, "
    . "   nvarchar2_t10, "
    . "   varchar_t10) "
    . " VALUES (:n1, "
    .         " :c1, "
    .         " :c2, "
    .         " :c3, "
    .         " :n2, "
    .         " :n3, "
    .         " :n4, "
    .         " to_date(:d1, 'YYYY-MM-DD HH24:MI:SS'), "
    .         " to_timestamp(:d1, 'YYYY-MM-DD HH24:MI:SS'), "
    .         " :n5, "
    .         " :n5, "
    .         " :n5, "
    .         " :n1, "
    .         " :n1, "
    .         " :c4, "
    .         " :c5, "
    .         " :c6) ";


$n1 = "86";
$c1 = "C";
$c2 = "char10";
$c3 = "varchar210";
$n2 = "-123.456";
$n3 = "789.346";
$n4 = "123456.023";
$n5 = "12345678901234567890123456789012345678.723";
$d1 = "2010-03-29 13:09:15";
$c4 = "nchar10";
$c5 = "nvarchar2x";
$c6 = "varchar";

$s = oci_parse($c, $insert_sql);
oci_bind_by_name($s, ":n1", $n1);
oci_bind_by_name($s, ":c1", $c1);
oci_bind_by_name($s, ":c2", $c2);
oci_bind_by_name($s, ":c3", $c3);
oci_bind_by_name($s, ":n2", $n2);
oci_bind_by_name($s, ":n3", $n3);
oci_bind_by_name($s, ":n4", $n4);
oci_bind_by_name($s, ":d1", $d1);
oci_bind_by_name($s, ":n5", $n5);
oci_bind_by_name($s, ":c4", $c4);
oci_bind_by_name($s, ":c5", $c5);
oci_bind_by_name($s, ":c6", $c6);

oci_execute($s);

echo "\nTEST87 SELECT all values using DEFINEs\n";
$select_sql = "select "
    . "id, "
    . "char_t, "
    . "char_t10, "
    . "varchar2_t10, "
    . "number_t, "
    . "number_t92, "
    . "number_t6, "
    . "date_t, "
    . "timestamp_t, "
    . "float_t, "
    . "binary_float_t, "
    . "binary_double_t, "
    . "decimal_t, "
    . "integer_t, "
    . "nchar_t, "
    . "nvarchar2_t10, "
    . "varchar_t10 "
    . "from bind_misccoltypes_tab where id = 86";

$s = oci_parse($c, $select_sql);

oci_define_by_name($s, "ID", $ID);
oci_define_by_name($s, "CHAR_T", $CHAR_T);
oci_define_by_name($s, "CHAR_T10", $CHAR_T10);
oci_define_by_name($s, "VARCHAR2_T10", $VARCHAR2_T10);
oci_define_by_name($s, "NUMBER_T", $NUMBER_T);
oci_define_by_name($s, "NUMBER_T92", $NUMBER_T92);
oci_define_by_name($s, "NUMBER_T6", $NUMBER_T6);
oci_define_by_name($s, "DATE_T", $DATE_T);
oci_define_by_name($s, "TIMESTAMP_T", $TIMESTAMP_T);
oci_define_by_name($s, "FLOAT_T", $FLOAT_T);
oci_define_by_name($s, "BINARY_FLOAT_T", $BINARY_FLOAT_T);
oci_define_by_name($s, "BINARY_DOUBLE_T", $BINARY_DOUBLE_T);
oci_define_by_name($s, "DECIMAL_T", $DECIMAL_T);
oci_define_by_name($s, "INTEGER_T", $INTEGER_T);
oci_define_by_name($s, "NCHAR_T", $NCHAR_T);
oci_define_by_name($s, "NVARCHAR2_T10", $NVARCHAR2_T10);
oci_define_by_name($s, "VARCHAR_T10", $VARCHAR_T10);

oci_execute($s);

while (oci_fetch($s)) {
    echo "ID is "              . "$ID\n";
    echo "CHAR_T is "          . "$CHAR_T\n";
    echo "CHAR_T10 is "        . "$CHAR_T10\n";
    echo "VARCHAR2_T10 is "    . "$VARCHAR2_T10\n";
    echo "NUMBER_T is "        . "$NUMBER_T\n";
    echo "NUMBER_T92 is "      . "$NUMBER_T92\n";
    echo "NUMBER_T6 is "       . "$NUMBER_T6\n";
    echo "DATE_T is "          . "$DATE_T\n";
    echo "TIMESTAMP_T is "     . "$TIMESTAMP_T\n";
    echo "FLOAT_T is "         . "$FLOAT_T\n";
    echo "BINARY_FLOAT_T is "  . "$BINARY_FLOAT_T\n";
    echo "BINARY_DOUBLE_T is " . "$BINARY_DOUBLE_T\n";
    echo "DECIMAL_T is "       . "$DECIMAL_T\n";
    echo "INTEGER_T is "       . "$INTEGER_T\n";
    echo "NCHAR_T is "         . "$NCHAR_T\n";
    echo "NVARCHAR2_T10 is "   . "$NVARCHAR2_T10\n";
    echo "VARCHAR_T10 is "     . "$VARCHAR_T10\n";
}

echo "\nTEST52 insert numbers\n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_tab (id, number_t92) VALUES (52, :n1)");
$n1 = 3;
oci_bind_by_name($s, ":n1", $n1);
oci_execute($s);

check_col($c, 'number_t92', 52);


echo "\nTEST53 insert numbers \n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_tab (id, number_t92) VALUES (53, :n1)");
$n1 = 8.67;
oci_bind_by_name($s, ":n1", $n1);
oci_execute($s);

check_col($c, 'number_t92', 53);


echo "\nTEST54 insert numbers \n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_tab (id, number_t) VALUES (54, :n1)");
$n1 = 4.67;
oci_bind_by_name($s, ":n1", $n1);
oci_execute($s);

check_col($c, 'number_t', 54);

echo "\nTEST55 insert numbers \n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_tab (id, number_t) VALUES (55, :n1)");
$n1 = "7.67";
oci_bind_by_name($s, ":n1", $n1);
oci_execute($s);

check_col($c, 'number_t', 55);

echo "\nTEST56 insert numbers \n";

$n1 = -5.67;

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_tab (id, number_t) VALUES (56, :n1)");
oci_bind_by_name($s, ":n1", $n1);
oci_execute($s);

check_col($c, 'number_t', 56);

echo "\nTEST58 insert a VARCHAR2\n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_tab (id, varchar2_t10) VALUES (58, :c2)");
$c2 = "Hood";
oci_bind_by_name($s, ":c2", $c2);
oci_execute($s);

check_col($c, 'varchar2_t10', 58);

echo "\nTEST59 insert a VARCHAR2\n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_tab (id, char_t10) VALUES (59, :c2)");
$c2 = "Hood";
oci_bind_by_name($s, ":c2", $c2);
oci_execute($s);

check_col($c, 'char_t10', 59);

echo "\nTEST60 insert a date\n";

$s = oci_parse($c, "INSERT INTO bind_misccoltypes_tab (id, date_t) VALUES (60, to_date(:c2, 'YYYY-MM-DD'))");
$c2 = '2010-04-09';
oci_bind_by_name($s, ":c2", $c2);
oci_execute($s);

check_col($c, 'date_t', 60);


// Clean up

$stmtarray = array(
	"drop table bind_misccoltypes_tab"
);

oci8_test_sql_execute($c, $stmtarray);

oci_close($c);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
TEST86 insert all ORATYPE values

TEST87 SELECT all values using DEFINEs
ID is 86
CHAR_T is C
CHAR_T10 is char10    
VARCHAR2_T10 is varchar210
NUMBER_T is -123.456
NUMBER_T92 is 789.35
NUMBER_T6 is 123456
DATE_T is 29-MAR-10
TIMESTAMP_T is 29-MAR-10 01.09.15.000000 PM
FLOAT_T is 12345678901234567890123456789012345679
BINARY_FLOAT_T is 1.23456784E+037
BINARY_DOUBLE_T is 1.2345678901234568E+037
DECIMAL_T is 86
INTEGER_T is 86
NCHAR_T is nchar10   
NVARCHAR2_T10 is nvarchar2x
VARCHAR_T10 is varchar

TEST52 insert numbers
array(1) {
  ["NUMBER_T92"]=>
  array(1) {
    [0]=>
    string(1) "3"
  }
}

TEST53 insert numbers 
array(1) {
  ["NUMBER_T92"]=>
  array(1) {
    [0]=>
    string(4) "8.67"
  }
}

TEST54 insert numbers 
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(4) "4.67"
  }
}

TEST55 insert numbers 
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(4) "7.67"
  }
}

TEST56 insert numbers 
array(1) {
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(5) "-5.67"
  }
}

TEST58 insert a VARCHAR2
array(1) {
  ["VARCHAR2_T10"]=>
  array(1) {
    [0]=>
    string(4) "Hood"
  }
}

TEST59 insert a VARCHAR2
array(1) {
  ["CHAR_T10"]=>
  array(1) {
    [0]=>
    string(10) "Hood      "
  }
}

TEST60 insert a date
array(1) {
  ["DATE_T"]=>
  array(1) {
    [0]=>
    string(9) "09-APR-10"
  }
}
===DONE===
