--TEST--
mysqli fetch user variable
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

    mysqli_query($link, "SET @dummy='foobar'");

    $stmt = mysqli_prepare($link, "SELECT @dummy");

    mysqli_stmt_bind_result($stmt, $dummy);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);

    var_dump($dummy);

    mysqli_stmt_close($stmt);
    mysqli_close($link);
    print "done!";
?>
--EXPECT--
string(6) "foobar"
done!
