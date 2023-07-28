--TEST--
function test: mysqli_character_set_name
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

    $cset = substr(mysqli_character_set_name($link),0,6);

    var_dump($cset);

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
string(6) "utf8mb"
done!
