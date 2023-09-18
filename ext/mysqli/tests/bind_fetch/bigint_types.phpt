--TEST--
mysqli fetch bigint values (ok to fail with 4.1.x)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
if (PHP_INT_SIZE == 8) {
    echo 'skip test valid only for 32bit systems';
    exit;
}
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
mysqli_check_skip_test();
?>
--FILE--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";

$link = default_mysqli_connect();

// To get consistent result without depending on the DB version/setup
mysqli_query($link, "SET sql_mode=''");

mysqli_query(
    $link,
    "CREATE TABLE test_bind_fetch_integers_big(
        c1 bigint default 5,
        c2 bigint,
        c3 bigint not NULL,
        c4 bigint unsigned,
        c5 bigint unsigned,
        c6 bigint unsigned,
        c7 bigint unsigned,
        c8 bigint unsigned
    ) ENGINE=" . get_default_db_engine()
);

mysqli_query($link, "INSERT INTO test_bind_fetch_integers_big (c2,c3,c4,c5,c6,c7,c8)
    VALUES (-23,4.0,33333333333333,0,-333333333333,99.9,1234)");

$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_integers_big");
mysqli_stmt_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7, $c8);
mysqli_stmt_execute($stmt);
mysqli_stmt_fetch($stmt);

if (mysqli_get_server_version($link) < 50000) {
    // 4.1 is faulty and will return big number for $c6
    if ($c6 == "18446743740376218283") {
        $c6 = 0;
    }
}
$c8 = 4567;// change this to test how mysqli/mysqlnd handles is_ref changing
$test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8);

var_dump($test);

mysqli_stmt_close($stmt);
mysqli_close($link);
print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_bind_fetch_integers_big');
?>
--EXPECT--
array(8) {
  [0]=>
  int(5)
  [1]=>
  int(-23)
  [2]=>
  int(4)
  [3]=>
  string(14) "33333333333333"
  [4]=>
  int(0)
  [5]=>
  int(0)
  [6]=>
  int(100)
  [7]=>
  int(4567)
}
done!
