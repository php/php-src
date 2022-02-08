--TEST--
mysqli_stmt_affected_rows()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, 'DROP TABLE IF EXISTS test') ||
        !mysqli_stmt_execute($stmt)) {
        printf("[003] Failed to drop old test table: [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    }

    if (!mysqli_stmt_prepare($stmt, 'CREATE TABLE test(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE = ' . $engine) ||
        !mysqli_stmt_execute($stmt)) {
        printf("[004] Failed to create test table: [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    }

    if (0 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[005] Expecting int/0, got %s/'%s'\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (1, 'a')") ||
        !mysqli_stmt_execute($stmt))
        printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (100, 'z')") ||
        !mysqli_stmt_execute($stmt))
        printf("[007] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[008] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (100, 'z')") ||
        !mysqli_stmt_execute($stmt))
        // NOTE: the error message varies with the MySQL Server version, dump only the error code!
        printf("[009] [%d] (error message varies with the MySQL Server version, check the error code)\n", mysqli_stmt_errno($stmt));

    /* an error occurred: affected rows should return -1 */
    if (-1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[010] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (1, 'a') ON DUPLICATE KEY UPDATE id = 4") ||
        !mysqli_stmt_execute($stmt))
        printf("[011] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (2 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[012] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (2, 'b'), (3, 'c')") ||
        !mysqli_stmt_execute($stmt))
        printf("[013] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (2 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[014] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "INSERT IGNORE INTO test(id, label) VALUES (1, 'a')") ||
        !mysqli_stmt_execute($stmt))
        printf("[015] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[016] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    if (!($res = mysqli_query($link, "SELECT count(id) AS num FROM test")) ||
        !($tmp = mysqli_fetch_assoc($res)))
        printf("[017] [%d] %s\n", mysqli_error($link), mysqli_error($link));
    $num = (int)$tmp['num'];
    mysqli_free_result($res);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) SELECT id + 10, label FROM test") ||
        !mysqli_stmt_execute($stmt))
        printf("[018] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if ($num !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[019] Expecting int/%d, got %s/%s\n", $num, gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "REPLACE INTO test(id, label) values (4, 'd')") ||
        !mysqli_stmt_execute($stmt))
        printf("[020] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (2 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[021] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "REPLACE INTO test(id, label) values (5, 'e')") ||
        !mysqli_stmt_execute($stmt))
        printf("[022] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[023] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "UPDATE test SET label = 'a' WHERE id = 2") ||
        !mysqli_stmt_execute($stmt))
        printf("[024] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[025] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, "UPDATE test SET label = 'a' WHERE id = 2") ||
        !mysqli_stmt_execute($stmt))
        printf("[026] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (0 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[027] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "UPDATE test SET label = 'a' WHERE id = 100") ||
        !mysqli_stmt_execute($stmt))
        printf("[028] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[029] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, 'SELECT label FROM test WHERE id = 100') ||
        !mysqli_stmt_execute($stmt))
        printf("[030] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    /* use it like num_rows */
    /* PS are unbuffered, num_rows cannot determine the row count before all rows have been fetched and/or buffered */
    if (-1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[031] Expecting int/-1, got %s/%s\n", gettype($tmp), $tmp);

    if (0 !== ($tmp = mysqli_stmt_num_rows($stmt)))
        printf("[032] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_store_result($stmt))
        printf("[033] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[034] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (1 !== ($tmp = mysqli_stmt_num_rows($stmt)))
        printf("[035] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_free_result($stmt);
    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, 'SELECT label FROM test WHERE 1 = 2') ||
        !mysqli_stmt_execute($stmt))
        printf("[036] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    /* use it like num_rows */
    if (-1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[037] Expecting int/-1, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== ($tmp = mysqli_stmt_store_result($stmt)))
        printf("[038] Expecting boolean/true, got %s\%s\n", gettype($tmp), $tmp);

    if (0 !== ($tmp = mysqli_stmt_num_rows($stmt)))
        printf("[039] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (0 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[040] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    /* try to use stmt_affected_rows like stmt_num_rows */
    /* stmt_affected_rows is not really meant for SELECT! */
    if (mysqli_stmt_prepare($stmt, 'SELECT unknown_column FROM this_table_does_not_exist') ||
        mysqli_stmt_execute($stmt))
        printf("[041] Expecting SELECT statement to fail on purpose\n");

    if (-1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[042] Expecting int/-1, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== ($tmp = mysqli_stmt_store_result($stmt)))
        printf("[043] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    if (0 !== ($tmp = mysqli_stmt_num_rows($stmt)))
        printf("[044] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

    if (-1 !== ($tmp = mysqli_stmt_affected_rows($stmt)))
        printf("[045] Expecting int/-1, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_stmt_close($stmt);
    $stmt = mysqli_stmt_init($link);

    if (!mysqli_stmt_prepare($stmt, "DROP TABLE IF EXISTS test") ||
        !mysqli_stmt_execute($stmt))
        printf("[046] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    mysqli_stmt_close($stmt);

    try {
        mysqli_stmt_affected_rows($stmt);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
[009] [%d] (error message varies with the MySQL Server version, check the error code)
mysqli_stmt object is already closed
done!
