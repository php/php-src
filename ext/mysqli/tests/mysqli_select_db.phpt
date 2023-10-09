--TEST--
mysqli_select_db()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';
    require_once 'table.inc';

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    /* does not make too much sense, unless we have access to at least one more database than $db */
    if (!mysqli_select_db($link, $db))
        printf("[005] Cannot select DB %s, [%d] %s\n", $db, mysqli_errno($link), mysqli_error($link));

    if (!$res = mysqli_query($link, "SELECT DATABASE() AS dbname"))
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($row['dbname'] !== (string)$db)
        printf("[008] Expecting database '%s', found '%s'\n", $db, $row['dbname']);
    mysqli_free_result($res);

    if (mysqli_select_db($link, 'mysql')) {
        // Yippie, a second database to play with - that's great because mysqli_select_db
        // ($db) was done by mysqli__connect() already and the previous test
        // was quite useless
        if (!$res = mysqli_query($link, "SELECT DATABASE() AS dbname"))
            printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

        if (!$row = mysqli_fetch_assoc($res))
            printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

        if (strtolower($row['dbname']) !== 'mysql')
            printf("[011] Expecting database 'mysql', found '%s'\n", $row['dbname']);
        mysqli_free_result($res);
    }

    if (!$link->select_db($db))
        printf("[012] Failed to set '%s' as current DB; [%d] %s\n", $link->errno, $link->error);

    if (!$res = mysqli_query($link, "SELECT DATABASE() AS dbname"))
        printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $current_db = $row['dbname'];

    mysqli_report(MYSQLI_REPORT_OFF);
    mysqli_select_db($link, 'I cannot imagine that this database exists');
    mysqli_report(MYSQLI_REPORT_ERROR);

    ob_start();
    mysqli_select_db($link, 'I cannot imagine that this database exists');
    $output = ob_get_contents();
    ob_end_clean();
    if (!stristr($output, "1049") && !stristr($output, "1044") && !stristr($output, "1045")) {
      /* Error: 1049 SQLSTATE: 42000 (ER_BAD_DB_ERROR) Message: Unknown database '%s'  */
      /* Error: 1044 SQLSTATE: 42000 (ER_DBACCESS_DENIED_ERROR) Message: Access denied for user '%s'@'%s' to database '%s' */
      /* Error: 1045 SQLSTATE: 28000 (ER_ACCESS_DENIED_ERROR) Message: Access denied for user '%s'@'%s' (using password: %s) */
      echo $output;
    }

    if (!$res = mysqli_query($link, "SELECT DATABASE() AS dbname"))
        printf("[015] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$row = mysqli_fetch_assoc($res))
        printf("[016] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (strtolower($row['dbname']) != strtolower($current_db))
        printf("[017] Current DB should not change if set fails\n");


    if (!$res = $link->query("SELECT id FROM test WHERE id = 1"))
        printf("[018] [%d] %s\n");

    $row = $res->fetch_assoc();
    $res->free();

    mysqli_close($link);

    try {
        mysqli_select_db($link, $db);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!\n";
?>
--CLEAN--
<?php require_once 'clean_table.inc'; ?>
--EXPECT--
mysqli object is already closed
done!
