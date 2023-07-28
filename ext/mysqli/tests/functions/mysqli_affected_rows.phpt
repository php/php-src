--TEST--
function test: mysqli_affected_rows
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

    mysqli_query($link, "CREATE TABLE test_mysqli_affected_rows (a INT)");
    mysqli_query($link, "INSERT INTO test_mysqli_affected_rows VALUES (1),(2),(3)");

    $afc = mysqli_affected_rows($link);

    var_dump($afc);

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_mysqli_affected_rows');
?>
--EXPECT--
int(3)
done!
