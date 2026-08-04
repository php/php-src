--TEST--
mysqli persistent connection does not restore a post-connect set_charset() after reuse
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

$host = 'p:' . $host;

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
if (!$link)
    printf("[001] Cannot connect\n");

/* the charset the connection starts with; no charset was requested for it */
$default = mysqli_character_set_name($link);
if ($default === 'latin1')
    printf("[002] Test needs a default charset other than latin1, got %s\n", $default);

/* change it after the connection has been established */
mysqli_set_charset($link, 'latin1');

$thread_id = mysqli_thread_id($link);
mysqli_close($link);

/* reuse fires COM_RESET_CONNECTION */
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
if (mysqli_thread_id($link) !== $thread_id)
    printf("[003] Expected the pooled connection to be reused\n");

/* set_charset() runs after connect, so the reset does not restore it */
if (mysqli_character_set_name($link) !== $default)
    printf("[004] Charset should be reset to %s, got %s\n", $default, mysqli_character_set_name($link));

/* mysqlnd's cached charset (used for escaping) must still agree with the server */
$server = mysqli_query($link, "SELECT @@session.character_set_connection")->fetch_row()[0];
if (mysqli_character_set_name($link) !== $server)
    printf("[005] Client charset %s disagrees with server %s\n", mysqli_character_set_name($link), $server);

mysqli_close($link);

echo "done!";
?>
--EXPECT--
done!
