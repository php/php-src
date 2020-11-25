--TEST--
mysqli_error()
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

    if (!is_null($tmp = @mysqli_error()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_error($link)))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
    }

    mysqli_query($link, "set names utf8");

    $tmp = mysqli_error($link);
    if (!is_string($tmp) || ('' !== $tmp))
        printf("[004] Expecting string/empty, got %s/%s. [%d] %s\n", gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));


    mysqli_query($link, 'SELECT * FROM няма_такава_таблица');
    $tmp = mysqli_error($link);
    var_dump(str_replace($db.".", "", $tmp));

    mysqli_close($link);

    var_dump(mysqli_error($link));

    print "done!";
?>
--EXPECTF--
string(%d) "Table 'няма_такава_таблица' doesn't exist"

Warning: mysqli_error(): Couldn't fetch mysqli in %s on line %d
bool(false)
done!
