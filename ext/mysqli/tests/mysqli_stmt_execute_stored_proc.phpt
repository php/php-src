--TEST--
mysqli_stmt_execute() - Stored Procedures
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf('skip Cannot connect to MySQL, [%d] %s.', mysqli_connect_errno(), mysqli_connect_error()));
}
if (mysqli_get_server_version($link) <= 50000) {
    die(sprintf('skip Needs MySQL 5.0+, found version %d.', mysqli_get_server_version($link)));
}
?>
--FILE--
<?php
    require_once('connect.inc');
    require_once('table.inc');

    if (!mysqli_query($link, 'DROP PROCEDURE IF EXISTS p'))
        printf("[009] [%d] %s.\n", mysqli_errno($link), mysqli_error($link));

    if (mysqli_real_query($link, 'CREATE PROCEDURE p(OUT ver_param VARCHAR(25)) BEGIN SELECT VERSION() INTO ver_param; END;')) {
        /* no result set, one output parameter */
        if (!$stmt = mysqli_prepare($link, 'CALL p(@version)'))
            printf("[011] Cannot prepare CALL, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

        if (!mysqli_stmt_execute($stmt))
            printf("[012] Cannot execute CALL, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        /* yes, I really want to call it twice! */
        if (!mysqli_stmt_execute($stmt))
            printf("[013] Cannot execute CALL, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!mysqli_stmt_close($stmt))
            printf("[014] Cannot close statement, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!$stmt = mysqli_prepare($link, 'SELECT @version AS _version'))
            printf("[015] Cannot prepare SELECT, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

        if (!mysqli_stmt_execute($stmt))
            printf("[016] Cannot execute SELECT, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        $version = 'unknown';
        if (!mysqli_stmt_bind_result($stmt, $version) ||
                !mysqli_stmt_fetch($stmt))
            printf("[017] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (($version == "unknown") || ($version == ""))
            printf("[018] Results seem wrong, got %s, [%d] %s\n",
                $version,
                mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        mysqli_stmt_free_result($stmt);
        mysqli_stmt_close($stmt);

    } else {
        printf("[010] Cannot create SP, [%d] %s.\n", mysqli_errno($link), mysqli_error($link));
    }


    if (function_exists('mysqli_stmt_get_result')) {

        if (!mysqli_query($link, 'DROP PROCEDURE IF EXISTS p'))
            printf("[019] [%d] %s.\n", mysqli_errno($link), mysqli_error($link));

        if (mysqli_real_query($link, 'CREATE PROCEDURE p(OUT ver_param VARCHAR(25)) BEGIN SELECT VERSION() INTO ver_param; END;')) {
            // no result set, one output parameter
            if (!$stmt = mysqli_prepare($link, 'CALL p(@version)'))
                printf("[020] Cannot prepare CALL, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

            if (!mysqli_stmt_execute($stmt))
                printf("[021] Cannot execute CALL, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

            if (!mysqli_stmt_close($stmt))
                printf("[022] Cannot close statement, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

            if (!$stmt = mysqli_prepare($link, 'SELECT @version AS _version'))
                printf("[023] Cannot prepare SELECT, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

            if (!mysqli_stmt_execute($stmt))
                printf("[024] Cannot execute SELECT, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

            if (!$res = mysqli_stmt_get_result($stmt))
                printf("[025] Cannot get result set, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

            if ((!($row = mysqli_fetch_assoc($res))) || ($row['_version'] == ""))
                printf("[026] Results seem wrong, got %s, [%d] %s\n",
                    $row['_version'],
                    mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

            mysqli_free_result($res);
            mysqli_stmt_close($stmt);

        } else {
            printf("[027] Cannot create SP, [%d] %s.\n", mysqli_errno($link), mysqli_error($link));
        }

    }

    if (!mysqli_query($link, 'DROP PROCEDURE IF EXISTS p'))
        printf("[028] [%d] %s.\n", mysqli_errno($link), mysqli_error($link));

    if (mysqli_real_query($link, 'CREATE PROCEDURE p(IN ver_in VARCHAR(25), OUT ver_out VARCHAR(25)) BEGIN SELECT ver_in INTO ver_out; END;')) {
        // no result set, one input parameter, output parameter
        // yes, I really do not want to bind input values...
        if (!$stmt = mysqli_prepare($link, "CALL p('myversion', @version)"))
            printf("[029] Cannot prepare CALL, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

        if (!mysqli_stmt_execute($stmt))
            printf("[030] Cannot execute CALL, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!mysqli_stmt_close($stmt))
            printf("[031] Cannot close statement, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!$stmt = mysqli_prepare($link, 'SELECT @version AS _version'))
            printf("[032] Cannot prepare SELECT, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

        if (!mysqli_stmt_execute($stmt))
            printf("[033] Cannot execute SELECT, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        $version = 'unknown';
        if (!mysqli_stmt_bind_result($stmt, $version) ||
                !mysqli_stmt_fetch($stmt))
            printf("[034] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if ($version != "myversion")
            printf("[035] Results seem wrong, got %s, [%d] %s\n",
                $version,
                mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        mysqli_stmt_free_result($stmt);
        mysqli_stmt_close($stmt);

    } else {
        printf("[036] Cannot create SP, [%d] %s.\n", mysqli_errno($link), mysqli_error($link));
    }

    if (!mysqli_query($link, 'DROP PROCEDURE IF EXISTS p'))
        printf("[037] [%d] %s.\n", mysqli_errno($link), mysqli_error($link));

    if (mysqli_real_query($link, 'CREATE PROCEDURE p(IN ver_in VARCHAR(25), OUT ver_out VARCHAR(25)) BEGIN SELECT ver_in INTO ver_out; END;')) {
        // no result set, one input parameter, output parameter
        // yes, I really do not want to bind input values...
        if (!$stmt = mysqli_prepare($link, 'CALL p(?, @version)'))
            printf("[038] Cannot prepare CALL, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

        $version = 'myversion';
        if (!mysqli_stmt_bind_param($stmt, 's', $version))
            printf("[039] Cannot bind input parameter, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!mysqli_stmt_execute($stmt))
            printf("[040] Cannot execute CALL, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!mysqli_stmt_close($stmt))
            printf("[041] Cannot close statement, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!$stmt = mysqli_prepare($link, 'SELECT @version AS _version'))
            printf("[042] Cannot prepare SELECT, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

        if (!mysqli_stmt_execute($stmt))
            printf("[043] Cannot execute SELECT, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        $version = 'unknown';
        if (!mysqli_stmt_bind_result($stmt, $version) ||
                !mysqli_stmt_fetch($stmt))
            printf("[044] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if ($version !== "myversion")
            printf("[045] Results seem wrong, got %s, [%d] %s\n",
                $version,
                mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        mysqli_stmt_free_result($stmt);
        mysqli_stmt_close($stmt);

    } else {
        printf("[046] Cannot create SP, [%d] %s.\n", mysqli_errno($link), mysqli_error($link));
    }

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

@mysqli_query($link, 'DROP PROCEDURE IF EXISTS p');

mysqli_close($link);
?>
--EXPECT--
done!
