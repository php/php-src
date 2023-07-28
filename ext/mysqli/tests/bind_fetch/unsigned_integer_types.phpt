--TEST--
mysqli fetch unsigned integer values
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

    if (!mysqli_query($link, "SET sql_mode=''"))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $rc = mysqli_query($link, "CREATE TABLE test_bind_fetch_uint(c1 integer unsigned, c2 integer unsigned) ENGINE=" . get_default_db_engine());
    if (!$rc)
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test_bind_fetch_uint (c1,c2) VALUES (20123456, 3123456789)"))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_uint");
    mysqli_stmt_bind_result($stmt, $c1, $c2);
    mysqli_stmt_execute($stmt);
    $rc = mysqli_stmt_fetch($stmt);

    echo $c1, "\n", $c2, "\n";

    mysqli_stmt_close($stmt);
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_bind_fetch_uint');
?>
--EXPECT--
20123456
3123456789
done!
