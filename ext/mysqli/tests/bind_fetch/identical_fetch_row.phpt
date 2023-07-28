--TEST--
mysqli binding resulting of a fetch and fetching a row directly produce same results
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

    $rc = mysqli_query($link, "CREATE TABLE test_bind_fetch_and_row_fetch(c1 tinyint, c2 smallint,
                                                        c3 int, c4 bigint,
                                                        c5 decimal(4,2), c6 double,
                                                        c7 varbinary(10),
                                                        c8 varchar(10)) ENGINE=" . get_default_db_engine());
    if (!$rc)
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test_bind_fetch_and_row_fetch VALUES(120,2999,3999,54,
                                                              2.6,58.89,
                                                              '206','6.7')"))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch_and_row_fetch");

    $c = array(0,0,0,0,0,0,0,0);
    $b_res= mysqli_stmt_bind_result($stmt, $c[0], $c[1], $c[2], $c[3], $c[4], $c[5], $c[6], $c[7]);
    mysqli_stmt_execute($stmt);
    mysqli_stmt_fetch($stmt);
    mysqli_stmt_close($stmt);

    $result = mysqli_query($link, "SELECT * FROM test_bind_fetch_and_row_fetch");
    $d = mysqli_fetch_row($result);
    mysqli_free_result($result);

    $test = "";
    for ($i=0; $i < count($c); $i++)
        $test .= ($c[$i] == $d[$i]) ? "1" : "0";
    if ($test == "11111111")
        echo "ok\n";
    else
        echo "error";

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_bind_fetch_and_row_fetch');
?>
--EXPECT--
ok
done!
