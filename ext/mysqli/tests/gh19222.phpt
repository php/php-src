--TEST--
GH-19222 (php hangs when auth_socket is set for the user)
--EXTENSIONS--
mysqli
--INI--
mysqlnd.net_read_timeout=5
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
if (!extension_loaded('openssl')) {
    die('skip mysqlnd is built without SSL support, so the caching_sha2_password plugin is not available');
}
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
if (str_contains($link->server_info, 'MariaDB') || $link->server_version < 80000) {
    die('skip MySQL 8.0+ is required for caching_sha2_password and auth_socket');
}
$status = $link->query("SELECT PLUGIN_STATUS FROM INFORMATION_SCHEMA.PLUGINS WHERE PLUGIN_NAME = 'auth_socket'")->fetch_row();
if ((!$status || $status[0] !== 'ACTIVE') && !$link->query("INSTALL PLUGIN auth_socket SONAME 'auth_socket.so'")) {
    die('skip Cannot install the auth_socket plugin: ' . $link->error);
}
$link->query("DROP USER IF EXISTS 'gh19222'@'localhost'");
if (!$link->query(sprintf("CREATE USER 'gh19222'@'localhost' IDENTIFIED WITH auth_socket AS '%s'", $link->real_escape_string(get_current_user())))) {
    die('skip Cannot create an auth_socket user: ' . $link->error);
}
// The server accepts an auth_socket login only over the Unix socket from the matching OS user
$conn = @mysqli_connect('localhost', 'gh19222', '', '', 0, $socket);
$link->query("DROP USER 'gh19222'@'localhost'");
if (!$conn) {
    die('skip Cannot log in as the auth_socket user over the Unix socket: ' . mysqli_connect_error());
}
?>
--FILE--
<?php
require_once 'connect.inc';

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$link->query(sprintf("CREATE USER 'gh19222'@'localhost' IDENTIFIED WITH auth_socket AS '%s'", $link->real_escape_string(get_current_user())));

// auth_socket ignores the password, so the server sends OK right after the handshake response
$conn = mysqli_connect('localhost', 'gh19222', 'password', '', 0, $socket);
var_dump($conn->query('SELECT CURRENT_USER()')->fetch_row()[0]);

var_dump($conn->change_user('gh19222', 'password', null));
var_dump($conn->query('SELECT CURRENT_USER()')->fetch_row()[0]);
?>
--CLEAN--
<?php
require_once 'connect.inc';
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$link->query("DROP USER IF EXISTS 'gh19222'@'localhost'");
?>
--EXPECT--
string(17) "gh19222@localhost"
bool(true)
string(17) "gh19222@localhost"
