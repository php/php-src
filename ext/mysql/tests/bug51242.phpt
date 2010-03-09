--TEST--
Bug #51242 (Empty mysql.default_port does not default to 3306 anymore, but 0)
--INI--
mysql.default_port=
mysql.default_socket=/this/does/not/really/need/to/exist
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifnotmysqlnd.inc');

// Specific to this bug: we need to be able to connect via TCP. We'll use
// 127.0.0.1:3306 as a (hopefully) moderately sensible default if the hostname
// is actually specified as localhost.
if ($host == 'localhost') {
    $host = '127.0.0.1';
}

if (!@mysql_connect("$host:3306", $user, $passwd)) {
    die("skip mysql not available at $host:3306");
}
?>
--FILE--
<?php
require_once('connect.inc');

if ($host == 'localhost') {
    $host = '127.0.0.1';
}

if ($link = my_mysql_connect($host, $user, $passwd, $db, null, $socket)) {
    var_dump($link);
} else {
    printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, null, $socket);
}
?>
--EXPECTF--
resource(%d) of type (mysql link)
