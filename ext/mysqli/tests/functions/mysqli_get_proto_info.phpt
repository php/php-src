--TEST--
function test: mysqli_get_proto_info
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

    $pinfo = mysqli_get_proto_info($link);

    var_dump($pinfo);

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
int(10)
done!
