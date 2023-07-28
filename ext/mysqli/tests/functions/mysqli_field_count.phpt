--TEST--
function test: mysqli_field_count()
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

    mysqli_select_db($link, get_default_database());

    mysqli_query($link, "CREATE TABLE test_mysqli_field_count (a int, b varchar(10)) ENGINE = " . get_default_db_engine());

    mysqli_query($link, "INSERT INTO test_mysqli_field_count VALUES (1, 'foo')");
    $ir[] = mysqli_field_count($link);

    mysqli_real_query($link, "SELECT * FROM test_mysqli_field_count");
    $ir[] = mysqli_field_count($link);


    var_dump($ir);

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once(dirname(__DIR__) . "/test_setup/test_helpers.inc");
tear_down_table_on_default_connection('test_mysqli_field_count');
?>
--EXPECT--
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(2)
}
done!
