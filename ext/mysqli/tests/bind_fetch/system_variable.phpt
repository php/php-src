--TEST--
mysqli fetch system variables
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

    if (!mysqli_query($link, "SET AUTOCOMMIT=0"))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$stmt = mysqli_prepare($link, "SELECT @@autocommit"))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    mysqli_stmt_bind_result($stmt, $c0);
    mysqli_stmt_execute($stmt);

    mysqli_stmt_fetch($stmt);

    var_dump($c0);

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
int(0)
done!
