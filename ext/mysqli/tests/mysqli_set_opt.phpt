--TEST--
mysqli_set_opt()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $link = mysqli_init();

    // print "run_tests.php don't fool me with your 'ungreedy' expression '.+?'!\n";
    var_dump(mysqli_set_opt($link, MYSQLI_READ_DEFAULT_GROUP, 'extra_my.cnf'));
    var_dump(mysqli_set_opt($link, MYSQLI_READ_DEFAULT_FILE, 'extra_my.cnf'));
    var_dump(mysqli_set_opt($link, MYSQLI_OPT_CONNECT_TIMEOUT, 10));
    var_dump(mysqli_set_opt($link, MYSQLI_OPT_LOCAL_INFILE, 1));
    var_dump(mysqli_set_opt($link, MYSQLI_INIT_COMMAND, 'SET AUTOCOMMIT=0'));
    var_dump(my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket));
    var_dump(mysqli_set_opt($link, MYSQLI_READ_DEFAULT_GROUP, 'extra_my.cnf'));
    var_dump(mysqli_set_opt($link, MYSQLI_READ_DEFAULT_FILE, 'extra_my.cnf'));
    var_dump(mysqli_set_opt($link, MYSQLI_OPT_CONNECT_TIMEOUT, 10));
    var_dump(mysqli_set_opt($link, MYSQLI_OPT_LOCAL_INFILE, 1));
    var_dump(mysqli_set_opt($link, MYSQLI_INIT_COMMAND, 'SET AUTOCOMMIT=0'));
    var_dump(mysqli_set_opt($link, MYSQLI_CLIENT_SSL, 'not an mysqli_option'));

    mysqli_close($link);

    try {
        mysqli_set_opt($link, MYSQLI_INIT_COMMAND, 'SET AUTOCOMMIT=1');
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
mysqli object is already closed
done!
