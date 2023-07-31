--TEST--
function test: mysqli_error
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

/* Disable exceptions */
mysqli_report(MYSQLI_REPORT_OFF);

    $link = default_mysqli_connect();
    $error = mysqli_error($link);
    var_dump($error);

    mysqli_query($link, "SELECT * FROM non_existing_table");
    $error = mysqli_error($link);

    var_dump($error);

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
// Re-enable exceptions
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
?>
--EXPECTF--
string(0) ""
string(%d) "%s"
done!
