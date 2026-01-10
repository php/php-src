--TEST--
function test: mysqli_get_host_info
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

    $hinfo = mysqli_get_host_info($link);

    var_dump(str_replace('/','', $hinfo));

    mysqli_close($link);
    print "done!";
?>
--EXPECTF--
string(%d) "%s via %s"
done!
