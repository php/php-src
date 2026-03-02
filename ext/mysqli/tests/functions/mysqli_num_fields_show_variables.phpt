--TEST--
function test: mysqli_num_fields() 2
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

    mysqli_real_query($link, "SHOW VARIABLES");

    if (mysqli_field_count($link)) {
        $result = mysqli_store_result($link);
        $num = mysqli_num_fields($result);
        mysqli_free_result($result);
    }

    var_dump($num);

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
int(2)
done!
