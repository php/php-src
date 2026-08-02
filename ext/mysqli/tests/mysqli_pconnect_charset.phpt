--TEST--
mysqli persistent connection restores set_charset() after COM_RESET_CONNECTION reuse
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

mysqli_set_charset($link, 'latin1');
if (mysqli_character_set_name($link) !== 'latin1')
    printf("[002] Expected latin1, got %s\n", mysqli_character_set_name($link));

$thread_id = mysqli_thread_id($link);
mysqli_close($link);

/* reuse fires COM_RESET_CONNECTION, which reverts the charset on the server */
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
if (mysqli_thread_id($link) !== $thread_id)
    printf("[003] Expected the pooled connection to be reused\n");

/* mysqlnd's cached charset (used for escaping) must still agree with the server */
if (mysqli_character_set_name($link) !== 'latin1')
    printf("[004] Client-side charset should still be latin1, got %s\n", mysqli_character_set_name($link));

$res = mysqli_query($link, "SELECT @@session.character_set_connection AS c");
$row = mysqli_fetch_assoc($res);
if ($row['c'] !== 'latin1')
    printf("[005] Server charset should be restored to latin1, got %s\n", $row['c']);
mysqli_free_result($res);

mysqli_close($link);

echo "done!";
?>
--EXPECT--
done!
