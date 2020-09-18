--TEST--
set character set
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_set_charset')) {
    die('skip mysqli_set_charset() not available');
}
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!$mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
        printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

    if (!mysqli_query($mysql, "SET sql_mode=''"))
        printf("[002] Cannot set SQL-Mode, [%d] %s\n", mysqli_errno($mysql), mysqli_error($mysql));

    $esc_str = chr(0xbf) . chr(0x5c);
    $len = $charset = array();
    $tmp = null;

    if ($mysql->set_charset("latin1")) {
        /* 5C should be escaped */
        if (3 !== ($tmp = strlen($mysql->real_escape_string($esc_str))))
            printf("[003] Expecting 3/int got %s/%s\n", gettype($tmp), $tmp);

        if ('latin1' !== ($tmp = $mysql->character_set_name()))
            printf("[004] Expecting latin1/string got %s/%s\n", gettype($tmp), $tmp);
    }

    if ($res = $mysql->query("SHOW CHARACTER SET LIKE 'gbk'")) {
        $res->free_result();
        if ($mysql->set_charset("gbk")) {
            /* nothing should be escaped, it's a valid gbk character */

            if (2 !== ($tmp = strlen($mysql->real_escape_string($esc_str))))
                    printf("[005] Expecting 2/int got %s/%s\n", gettype($tmp), $tmp);

            if ('gbk' !== ($tmp = $mysql->character_set_name()))
                    printf("[005] Expecting gbk/string got %s/%s\n", gettype($tmp), $tmp);
        }
    }
    $mysql->close();

    print "done!";
?>
--EXPECT--
done!
