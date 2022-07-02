--TEST--
mysqli_real_escape_string() - euckr
--EXTENSIONS--
mysqli
--SKIPIF--
<?php

require_once('skipifconnectfailure.inc');
require_once('connect.inc');

if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf("skip Cannot connect to MySQL, [%d] %s\n",
        mysqli_connect_errno(), mysqli_connect_error()));
}
if (!mysqli_set_charset($link, 'euckr'))
    die(sprintf("skip Cannot set charset 'euckr'"));
mysqli_close($link);
?>
--FILE--
<?php
    require_once("connect.inc");
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
{
        printf("[001] Cannot connect to the server using host=%s, user=%s,
passwd=***, dbname=%s, port=%s, socket=%s - [%d] %s\n", $host, $user, $db,
$port, $socket, mysqli_connect_errno(), mysqli_connect_error());
    }

    if (!mysqli_query($link, 'DROP TABLE IF EXISTS test')) {
        printf("Failed to drop old test table: [%d] %s\n", mysqli_errno($link),
mysqli_error($link));
    }

    if (!mysqli_query($link, 'CREATE TABLE test(id INT, label CHAR(1), PRIMARY
KEY(id)) ENGINE=' . $engine . " DEFAULT CHARSET=euckr")) {
        printf("Failed to create test table: [%d] %s\n", mysqli_errno($link),
mysqli_error($link));
    }

    var_dump(mysqli_set_charset($link, "euckr"));

    if ('�Ǵ뼺\\\\�Ǵ뼺' !== ($tmp = mysqli_real_escape_string($link, '�Ǵ뼺\\�Ǵ뼺')))
        printf("[004] Expecting \\\\, got %s\n", $tmp);

    if ('�Ǵ뼺\"�Ǵ뼺' !== ($tmp = mysqli_real_escape_string($link, '�Ǵ뼺"�Ǵ뼺')))
        printf("[005] Expecting \", got %s\n", $tmp);

    if ("�Ǵ뼺\'�Ǵ뼺" !== ($tmp = mysqli_real_escape_string($link, "�Ǵ뼺'�Ǵ뼺")))
        printf("[006] Expecting ', got %s\n", $tmp);

    if ("�Ǵ뼺\\n�Ǵ뼺" !== ($tmp = mysqli_real_escape_string($link, "�Ǵ뼺\n�Ǵ뼺")))
        printf("[007] Expecting \\n, got %s\n", $tmp);

    if ("�Ǵ뼺\\r�Ǵ뼺" !== ($tmp = mysqli_real_escape_string($link, "�Ǵ뼺\r�Ǵ뼺")))
        printf("[008] Expecting \\r, got %s\n", $tmp);

    if ("�Ǵ뼺\\0�Ǵ뼺" !== ($tmp = mysqli_real_escape_string($link, "�Ǵ뼺" . chr(0) . "�Ǵ뼺")))
        printf("[009] Expecting %s, got %s\n", "�Ǵ뼺\\0�Ǵ뼺", $tmp);

    var_dump(mysqli_query($link, "INSERT INTO test(id, label) VALUES (100, '��')"));

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
bool(true)
bool(true)
done!
