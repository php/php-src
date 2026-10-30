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

    // To get consistent result without depending on the DB version/setup
    mysqli_query($link, "SET sql_mode=''");

    mysqli_query($link, "CREATE TABLE test_bind_fetch_uint(c1 integer unsigned, c2 integer unsigned) ENGINE=" . get_default_db_engine());

    mysqli_query($link, "INSERT INTO test_bind_fetch_uint (c1,c2) VALUES (20123456, 3123456789)");

    $stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_uint");
    mysqli_stmt_bind_result($stmt, $c1, $c2);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);

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
