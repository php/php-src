--TEST--
Bug #78855 Native PHP types in database fetches
--SKIPIF--
<?php
include("skipif.inc");
?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$res = pg_query($db, "SELECT null, true, false, 1::smallint, 2::int, 3::oid, 'text', '\\x3031'::bytea, 1.1::real, 1.2::float, 'Infinity'::float, '-Infinity'::float, 'NaN'::float");
var_dump(pg_fetch_array($res, 0, PGSQL_NUM));
var_dump(pg_fetch_array($res, 0, PGSQL_NUM|PGSQL_TYPED));

var_dump(pg_fetch_all($res, PGSQL_NUM));
var_dump(pg_fetch_all($res, PGSQL_NUM|PGSQL_TYPED));

?>
--EXPECT--
array(13) {
  [0]=>
  NULL
  [1]=>
  string(1) "t"
  [2]=>
  string(1) "f"
  [3]=>
  string(1) "1"
  [4]=>
  string(1) "2"
  [5]=>
  string(1) "3"
  [6]=>
  string(4) "text"
  [7]=>
  string(6) "\x3031"
  [8]=>
  string(3) "1.1"
  [9]=>
  string(3) "1.2"
  [10]=>
  string(8) "Infinity"
  [11]=>
  string(9) "-Infinity"
  [12]=>
  string(3) "NaN"
}
array(13) {
  [0]=>
  NULL
  [1]=>
  bool(true)
  [2]=>
  bool(false)
  [3]=>
  int(1)
  [4]=>
  int(2)
  [5]=>
  int(3)
  [6]=>
  string(4) "text"
  [7]=>
  string(2) "01"
  [8]=>
  float(1.1)
  [9]=>
  float(1.2)
  [10]=>
  float(INF)
  [11]=>
  float(-INF)
  [12]=>
  float(NAN)
}
array(1) {
  [0]=>
  array(13) {
    [0]=>
    NULL
    [1]=>
    string(1) "t"
    [2]=>
    string(1) "f"
    [3]=>
    string(1) "1"
    [4]=>
    string(1) "2"
    [5]=>
    string(1) "3"
    [6]=>
    string(4) "text"
    [7]=>
    string(6) "\x3031"
    [8]=>
    string(3) "1.1"
    [9]=>
    string(3) "1.2"
    [10]=>
    string(8) "Infinity"
    [11]=>
    string(9) "-Infinity"
    [12]=>
    string(3) "NaN"
  }
}
array(1) {
  [0]=>
  array(13) {
    [0]=>
    NULL
    [1]=>
    bool(true)
    [2]=>
    bool(false)
    [3]=>
    int(1)
    [4]=>
    int(2)
    [5]=>
    int(3)
    [6]=>
    string(4) "text"
    [7]=>
    string(2) "01"
    [8]=>
    float(1.1)
    [9]=>
    float(1.2)
    [10]=>
    float(INF)
    [11]=>
    float(-INF)
    [12]=>
    float(NAN)
  }
}
