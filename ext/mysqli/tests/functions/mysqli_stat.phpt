--TEST--
function test: mysqli_stat
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

    $status = mysqli_stat($link);

    var_dump(strlen($status) > 0);

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
bool(true)
done!
