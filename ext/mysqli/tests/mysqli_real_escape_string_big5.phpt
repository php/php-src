--TEST--
mysqli_real_escape_string() - big5
--SKIPIF--
<?php

require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');

if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf("skip Cannot connect to MySQL, [%d] %s\n",
        mysqli_connect_errno(), mysqli_connect_error()));
}
if (!mysqli_set_charset($link, 'big5'))
    die(sprintf("skip Cannot set charset 'big5'"));
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
KEY(id)) ENGINE=' . $engine . " DEFAULT CHARSET=big5")) {
        printf("Failed to create test table: [%d] %s\n", mysqli_errno($link),
mysqli_error($link));
    }

    var_dump(mysqli_set_charset($link, "big5"));

    if ('找到以下內容\\\\找到以下內容' !== ($tmp = mysqli_real_escape_string($link,
'找到以下內容\\找到以下內容')))
        printf("[004] Expecting \\\\, got %s\n", $tmp);

    if ('找到以下內容\"找到以下內容' !== ($tmp = mysqli_real_escape_string($link,
'找到以下內容"找到以下內容')))
        printf("[005] Expecting \", got %s\n", $tmp);

    if ("找到以下內容\'找到以下內容" !== ($tmp = mysqli_real_escape_string($link,
"找到以下內容'找到以下內容")))
        printf("[006] Expecting ', got %s\n", $tmp);

    if ("找到以下內容\\n找到以下內容" !== ($tmp = mysqli_real_escape_string($link,
"找到以下內容\n找到以下內容")))
        printf("[007] Expecting \\n, got %s\n", $tmp);

    if ("找到以下內容\\r找到以下內容" !== ($tmp = mysqli_real_escape_string($link,
"找到以下內容\r找到以下內容")))
        printf("[008] Expecting \\r, got %s\n", $tmp);

    if ("找到以下內容\\0找到以下內容" !== ($tmp = mysqli_real_escape_string($link, "找到以下內容"
. chr(0) . "找到以下內容")))
        printf("[009] Expecting %s, got %s\n", "找到以下內容\\0找到以下內容", $tmp);

    var_dump(mysqli_query($link, "INSERT INTO test(id, label) VALUES (100,
'找')"));

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
