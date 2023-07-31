--TEST--
function test: mysqli_errno
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
    $errno = mysqli_errno($link);
    var_dump($errno);

    mysqli_query($link, "SELECT * FROM non_existing_table");
    $errno = mysqli_errno($link);

    var_dump($errno);

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
// Re-enable exceptions
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
?>
--EXPECT--
int(0)
int(1146)
done!
