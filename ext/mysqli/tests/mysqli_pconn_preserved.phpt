--TEST--
mysqli: reusing a persistent connection re-applies its connection config after reset
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
// COM_RESET_CONNECTION reverts the session to server defaults; mysqlnd should
// then re-apply the connection config, so a reused connection stays configured.

require_once 'connect.inc';

$host = 'p:' . $host;

// Open a persistent connection carrying both a charset and an init command.
function connect_configured($host, $user, $passwd, $db, $port, $socket) {
    $link = mysqli_init();
    mysqli_options($link, MYSQLI_SET_CHARSET_NAME, 'latin1');
    mysqli_options($link, MYSQLI_INIT_COMMAND, 'SET @init_marker = 42');

    if (!mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
        return false;
    }

    return $link;
}

// Establish a persistent handle with the above configuration.
$link = connect_configured($host, $user, $passwd, $db, $port, $socket);

if (!$link) {
    printf("[001] Cannot connect\n");
}

$thread_id = mysqli_thread_id($link);

// Sanity check that the charset was configured.
if (mysqli_character_set_name($link) !== 'latin1') {
    printf("[002] Charset should be latin1 at connect, got %s\n", mysqli_character_set_name($link));
}

// Sanity check that the init command was executed.
$marker = mysqli_query($link, "SELECT @init_marker")->fetch_row()[0];

if ($marker != 42) {
    printf("[003] Init command should have set @init_marker at connect, got %s\n", var_export($marker, true));
}

// Return the connection to the pool.
mysqli_close($link);

// Reopen the same persistent connection, which should be reset, but preserve
// the options from the initial configuration.
$link = connect_configured($host, $user, $passwd, $db, $port, $socket);

// Compare the thread IDs to ensure the connection was reused.
if (mysqli_thread_id($link) !== $thread_id) {
    printf("[004] Expected the pooled connection to be reused\n");
}

// Check the charset to ensure the configured one is back in effect.
if (mysqli_character_set_name($link) !== 'latin1') {
    printf("[005] Charset should be re-applied to latin1, got %s\n", mysqli_character_set_name($link));
}

// Check the server-side charset to ensure it agrees with the client.
$server = mysqli_query($link, "SELECT @@session.character_set_connection")->fetch_row()[0];

if ($server !== 'latin1') {
    printf("[006] Server charset should be latin1, got %s\n", $server);
}

// Check the session variable to ensure the init command was re-executed.
$marker = mysqli_query($link, "SELECT @init_marker")->fetch_row()[0];

if ($marker != 42) {
    printf("[007] Init command should be re-run after reuse, got %s\n", var_export($marker, true));
}

mysqli_close($link);

echo "done!";
?>
--EXPECT--
done!
