--TEST--
function test: mysqli_get_server_info
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

    $sinfo = mysqli_get_server_info($link);

    /* Version is in format: X.Y.Z */
    $status = preg_match("#^\d+\.\d+\.\d+$#", $sinfo) === 1;
    var_dump($status);
    if (!$status) {
        var_dump($sinfo);
    }

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
bool(true)
done!
