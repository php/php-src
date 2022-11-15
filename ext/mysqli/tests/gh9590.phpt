--TEST--
Bug GH-9602 (stream_select does not abort upon exception or empty valid fd set)
--EXTENSIONS--
mysqli
posix
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    die("skip cannot connect");

if (mysqli_get_server_version($link) < 50012)
    die("skip Test needs SQL function SLEEP() available as of MySQL 5.0.12");

if (!function_exists('posix_setrlimit') || !posix_setrlimit(POSIX_RLIMIT_NOFILE, 2048, -1))
    die('skip Failed to set POSIX_RLIMIT_NOFILE');
?>
--FILE--
<?php
    posix_setrlimit(POSIX_RLIMIT_NOFILE, 2048, -1);

    $fds = [];
    for ($i = 0; $i < 1023; $i++) {
        $fds[] = @fopen(__DIR__ . "/GH-9590-tmpfile.$i", 'w');
    }

    require_once('connect.inc');

    function get_connection() {
        global $host, $user, $passwd, $db, $port, $socket;

        if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
            printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
        return $link;
    }


    $mysqli1 = get_connection();
    $mysqli2 = get_connection();

    var_dump(mysqli_query($mysqli1, "SELECT SLEEP(0.10)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));
    var_dump(mysqli_query($mysqli2, "SELECT SLEEP(0.20)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));

    $links = $errors = $reject = array($mysqli1, $mysqli2);
    var_dump(mysqli_poll($links, $errors, $reject, 0, 50000));

    mysqli_close($mysqli1);
    mysqli_close($mysqli2);

    print "done!";
?>
--EXPECTF--
bool(true)
bool(true)

Warning: mysqli_poll(): You MUST recompile PHP with a larger value of FD_SETSIZE.
It is set to 1024, but you have descriptors numbered at least as high as %d.
 --enable-fd-setsize=%d is recommended, but you may want to set it
to equal the maximum number of open files supported by your system,
in order to avoid seeing this error again at a later date. in %s on line %d
bool(false)
done!
--CLEAN--
<?php
for ($i = 0; $i < 1023; $i++) {
    @unlink(__DIR__ . "/GH-9590-tmpfile.$i");
}
?>
