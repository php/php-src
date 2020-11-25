--TEST--
mysqli_dump_debug_info()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $tmp	= NULL;
    $link	= NULL;

    if (!$mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n", $host, $user, $db, $port, $socket);

    if (NULL !== ($tmp = @$mysqli->dump_debug_info($link)))
        printf("[002] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_bool($tmp = $mysqli->dump_debug_info()))
        printf("[003] Expecting boolean/[true|false] value, got %s/%s, [%d] %s\n",
            gettype($tmp), $tmp,
            $mysqli->errno, $mysqli->error);

    $mysqli->close();

    if (false !== ($tmp = $mysqli->dump_debug_info()))
        printf("[004] Expecting false, got %s/%s, [%d] %s\n",
            gettype($tmp), $tmp,
            $mysqli->errno, $mysqli->error);

    print "done!";
?>
--EXPECTF--
Warning: mysqli::dump_debug_info(): Couldn't fetch mysqli in %s on line %d
done!
