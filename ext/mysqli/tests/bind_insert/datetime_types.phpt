--TEST--
mysqli insert (bind_param + bind_result) datetime types
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
mysqli_check_skip_test();
?>
--FILE--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";

$link = default_mysqli_connect();

// To get consistent result without depending on the DB version/setup
mysqli_query($link, "SET sql_mode=''");

/* 14 Too big precision for timestamp */
mysqli_query(
    $link,
    "CREATE TABLE insert_bind_datetime(
        c1 date,
        c2 time,
        c3 timestamp,
        c4 year,
        c5 datetime,
        c6 timestamp,
        c7 timestamp
    )"
);

$stmt = mysqli_prepare($link, "INSERT INTO insert_bind_datetime VALUES (?,?,?,?,?,?,?)");
mysqli_stmt_bind_param($stmt, "sssssss", $d1, $d2, $d3, $d4, $d5, $d6, $d7);

$d1 = "2002-01-02";
$d2 = "12:49:00";
$d3 = "2002-01-02 17:46:59";
$d4 = "2010";
$d5 = "2010-07-10";
$d6 = "2020";
$d7 = "1999-12-29";

mysqli_stmt_execute($stmt);
mysqli_stmt_close($stmt);

$stmt = mysqli_prepare($link, "SELECT c1, c2, c3, c4, c5, c6, c7 FROM insert_bind_datetime");

mysqli_stmt_bind_result($stmt,$c1, $c2, $c3, $c4, $c5, $c6, $c7);

mysqli_stmt_execute($stmt);
mysqli_stmt_fetch($stmt);

$test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7);

var_dump($test);

mysqli_stmt_close($stmt);
mysqli_close($link);

print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('insert_bind_datetime');
?>
--EXPECTF--
array(7) {
  [0]=>
  %s(10) "2002-01-02"
  [1]=>
  %s(8) "12:49:00"
  [2]=>
  %s(19) "2002-01-02 17:46:59"
  [3]=>
  string(4) "2010"
  [4]=>
  %s(19) "2010-07-10 00:00:00"
  [5]=>
  %s(19) "0000-00-00 00:00:00"
  [6]=>
  %s(19) "1999-12-29 00:00:00"
}
done!
