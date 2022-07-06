--TEST--
mysqli_stmt_execute() - OUT
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf('skip Cannot connect to MySQL, [%d] %s.', mysqli_connect_errno(), mysqli_connect_error()));
}
if (mysqli_get_server_version($link) < 50503) {
    die(sprintf('skip Needs MySQL 5.5.3+, found version %d.', mysqli_get_server_version($link)));
}
/*
if ($IS_MYSQLND) {
    die(sprintf("skip WHY ?!"));
}
*/
?>
--FILE--
<?php
    require_once('connect.inc');

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
          $host, $user, $db, $port, $socket);
    }

    if (!mysqli_query($link, 'DROP PROCEDURE IF EXISTS p'))
        printf("[003] [%d] %s.\n", mysqli_errno($link), mysqli_error($link));

    if (mysqli_real_query($link, 'CREATE PROCEDURE p(IN ver_in VARCHAR(25), OUT ver_out VARCHAR(25)) BEGIN SELECT ver_in INTO ver_out; END;')) {
        if (!$stmt = mysqli_prepare($link, 'CALL p(?, ?)'))
            printf("[005] Cannot prepare CALL, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

        $ver_in = 'myversion';
        $ver_out = '';
        if (!mysqli_stmt_bind_param($stmt, 'ss', $ver_in, $ver_out))
            printf("[006] Cannot bind parameter, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!mysqli_stmt_execute($stmt))
            printf("[007] Cannot execute CALL, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        printf("[008] More results: %s\n", (mysqli_more_results($link) ? "yes" : "no"));
        printf("[009] Next results: %s\n", (mysqli_next_result($link) ? "yes" : "no"));

        try {
            if (!mysqli_stmt_bind_result($stmt, $ver_out) || !mysqli_stmt_fetch($stmt))
                printf("[010] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
        } catch (\ArgumentCountError $e) {
            echo $e->getMessage() . \PHP_EOL;
        }

        if ("myversion" !== $ver_out)
            printf("[011] Results seem wrong got '%s'\n", $ver_out);

        if (!mysqli_stmt_close($stmt))
            printf("[012] Cannot close statement, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        if (!$res = $link->query("SELECT 1"))
            printf("[013] [%d] %s\n", $link->errno, $link->error);

    } else {
        printf("[004] Cannot create SP, [%d] %s.\n", mysqli_errno($link), mysqli_error($link));
    }

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

@mysqli_query($link, 'DROP PROCEDURE IF EXISTS p');

mysqli_close($link);
?>
--XFAIL--
Unsupported and undefined, under development
--EXPECTF--
[008] More results: %s
[009] Next results: %s
done!
