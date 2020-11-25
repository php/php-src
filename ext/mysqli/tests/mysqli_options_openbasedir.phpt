--TEST--
mysqli_options() - MYSQLI_OPT_LOCAL_INFILE and open_basedir
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once('connect.inc');
ini_set("open_basedir", __DIR__);
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    printf("[001] Cannot connect, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if ($IS_MYSQLND) {
    if (true !== mysqli_options($link, MYSQLI_OPT_LOCAL_INFILE, 1))
        printf("[002] Can not set MYSQLI_OPT_LOCAL_INFILE although open_basedir is set!\n");

} else {
    if (false !== mysqli_options($link, MYSQLI_OPT_LOCAL_INFILE, 1))
        printf("[002] Can set MYSQLI_OPT_LOCAL_INFILE although open_basedir is set!\n");

}
mysqli_close($link);
print "done!";
?>
--EXPECT--
done!
