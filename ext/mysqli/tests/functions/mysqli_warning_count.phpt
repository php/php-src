--TEST--
function test: mysqli_warning_count()
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

    mysqli_query($link, "DROP TABLE IF EXISTS test_mysqli_warning_count");
    mysqli_query($link, "DROP TABLE IF EXISTS test_mysqli_warning_count");

    var_dump(mysqli_warning_count($link));

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_mysqli_warning_count');
?>
--EXPECT--
int(1)
done!
