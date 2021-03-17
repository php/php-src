--TEST--
PDO_Firebird: getColumnMeta
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php

require("testdb.inc");

@$dbh->exec('DROP TABLE testcolmeta');
$dbh->exec('CREATE TABLE testcolmeta (
    col_notnull         CHAR(30) not null,
    col_char            CHAR(20),
    col_varchar         VARCHAR(20),
    col_smallint        SMALLINT,
    col_integer         INTEGER,
    col_bigint          BIGINT,
    col_decimal_short   DECIMAL(2,1),
    col_decimal_int     DECIMAL(6,1),
    col_float           FLOAT,
    col_date            DATE,
    col_time            TIME,
    col_timestamp       TIMESTAMP
)');
$dbh->exec("INSERT INTO testcolmeta VALUES (
    'col_notnull', 'col_char', 'col_varchar',
    1, 2, 3, 2.1, 6.1, 1234.1234,
    '2020-01-02', '10:15:30', '2020-03-04 11:16:40')");
$dbh->commit();

$query = "
    SELECT
        col_notnull,
        col_char,
        col_varchar,
        col_smallint,
        col_integer,
        col_bigint,
        col_decimal_short,
        col_decimal_int,
        col_float,
        col_date,
        col_time,
        col_timestamp
    from testcolmeta";

$stmt = $dbh->prepare($query);
$stmt->execute(array());
$metas = array();
for ($i = 0; $i < 13; $i += 1) {
    $meta = $stmt->getColumnMeta($i);
    if (is_array($meta)) {
        ksort($meta);
    }
    $metas []= $meta;
}
var_dump($metas);
$dbh->commit();

unset($stmt);
unset($dbh);
?>
--EXPECT--
array(13) {
  [0]=>
  array(6) {
    ["flags"]=>
    array(1) {
      [0]=>
      string(8) "not_null"
    }
    ["len"]=>
    int(30)
    ["name"]=>
    string(11) "COL_NOTNULL"
    ["native_type"]=>
    string(4) "char"
    ["pdo_type"]=>
    int(3)
    ["precision"]=>
    int(0)
  }
  [1]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(20)
    ["name"]=>
    string(8) "COL_CHAR"
    ["native_type"]=>
    string(4) "char"
    ["pdo_type"]=>
    int(3)
    ["precision"]=>
    int(0)
  }
  [2]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(20)
    ["name"]=>
    string(11) "COL_VARCHAR"
    ["native_type"]=>
    string(7) "varchar"
    ["pdo_type"]=>
    int(3)
    ["precision"]=>
    int(0)
  }
  [3]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(2)
    ["name"]=>
    string(12) "COL_SMALLINT"
    ["native_type"]=>
    string(8) "smallint"
    ["pdo_type"]=>
    int(2)
    ["precision"]=>
    int(0)
  }
  [4]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(4)
    ["name"]=>
    string(11) "COL_INTEGER"
    ["native_type"]=>
    string(7) "integer"
    ["pdo_type"]=>
    int(2)
    ["precision"]=>
    int(0)
  }
  [5]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(8)
    ["name"]=>
    string(10) "COL_BIGINT"
    ["native_type"]=>
    string(6) "bigint"
    ["pdo_type"]=>
    int(2)
    ["precision"]=>
    int(0)
  }
  [6]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(4)
    ["name"]=>
    string(17) "COL_DECIMAL_SHORT"
    ["native_type"]=>
    string(7) "decimal"
    ["pdo_type"]=>
    int(3)
    ["precision"]=>
    int(1)
  }
  [7]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(4)
    ["name"]=>
    string(15) "COL_DECIMAL_INT"
    ["native_type"]=>
    string(7) "decimal"
    ["pdo_type"]=>
    int(3)
    ["precision"]=>
    int(1)
  }
  [8]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(4)
    ["name"]=>
    string(9) "COL_FLOAT"
    ["native_type"]=>
    string(5) "float"
    ["pdo_type"]=>
    int(3)
    ["precision"]=>
    int(0)
  }
  [9]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(4)
    ["name"]=>
    string(8) "COL_DATE"
    ["native_type"]=>
    string(4) "date"
    ["pdo_type"]=>
    int(3)
    ["precision"]=>
    int(0)
  }
  [10]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(4)
    ["name"]=>
    string(8) "COL_TIME"
    ["native_type"]=>
    string(4) "time"
    ["pdo_type"]=>
    int(3)
    ["precision"]=>
    int(0)
  }
  [11]=>
  array(6) {
    ["flags"]=>
    array(0) {
    }
    ["len"]=>
    int(8)
    ["name"]=>
    string(13) "COL_TIMESTAMP"
    ["native_type"]=>
    string(9) "timestamp"
    ["pdo_type"]=>
    int(3)
    ["precision"]=>
    int(0)
  }
  [12]=>
  bool(false)
}
