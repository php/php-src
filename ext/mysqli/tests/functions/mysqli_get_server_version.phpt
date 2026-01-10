--TEST--
mysqli_get_server_version
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

    $i = mysqli_get_server_version($link);

    $test = $i / $i;

    var_dump($test);

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
int(1)
done!
