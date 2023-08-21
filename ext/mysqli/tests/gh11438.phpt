--TEST--
GH-11438 (mysqlnd fails to authenticate with sha256_password accounts using passwords longer than 19 characters)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';

ob_start();
phpinfo(INFO_MODULES);
$tmp = ob_get_contents();
ob_end_clean();
if (!stristr($tmp, "auth_plugin_sha256_password"))
    die("skip SHA256 auth plugin not built-in to mysqlnd");

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    die(printf("skip: [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error()));

if (mysqli_get_server_version($link) < 50606)
    die("skip: SHA-256 requires MySQL 5.6.6+");

if (!($res = $link->query("SHOW PLUGINS"))) {
    die(sprintf("skip [%d] %s\n", $link->errno, $link->error));
}

$found = false;
while ($row = $res->fetch_assoc()) {
    if (($row['Name'] == 'sha256_password') && ($row['Status'] == 'ACTIVE')) {
        $found = true;
        break;
    }
}
if (!$found)
    die("skip SHA-256 server plugin unavailable");

// Ignore errors because this variable exists only in MySQL 5.6 and 5.7
$link->query("SET @@session.old_passwords=2");

$link->query('DROP USER shatest');
$link->query("DROP USER shatest@localhost");

if (!$link->query('CREATE USER shatest@"%" IDENTIFIED WITH sha256_password') ||
    !$link->query('CREATE USER shatest@"localhost" IDENTIFIED WITH sha256_password')) {
    die(sprintf("skip CREATE USER failed [%d] %s", $link->errno, $link->error));
}

// Password of length 52, more than twice the length of the scramble data to ensure scramble is repeated correctly
if (!$link->query('SET PASSWORD FOR shatest@"%" = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"') ||
    !$link->query('SET PASSWORD FOR shatest@"localhost" = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"')) {
    die(sprintf("skip SET PASSWORD failed [%d] %s", $link->errno, $link->error));
}

echo "nocache";
?>
--FILE--
<?php
require_once 'connect.inc';

$link = new mysqli($host, 'shatest', 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ', null, $port, $socket);
if ($link->connect_errno) {
    printf("[001] [%d] %s\n", $link->connect_errno, $link->connect_error);
} else {
    if (!$res = $link->query("SELECT USER()"))
        printf("[002] [%d] %s\n", $link->errno, $link->error);

    if (!$row = mysqli_fetch_assoc($res)) {
        printf("[003] [%d] %s\n", $link->errno, $link->error);
    }

    if (!is_string($row['USER()']) || !str_starts_with($row['USER()'], 'shatest')) {
        printf("[004] Expecting 1 got %s/'%s'", gettype($row['USER()']), $row['USER()']);
    }
}

print "done!";
?>
--CLEAN--
<?php
require_once 'connect.inc';
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    printf("[clean] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
        $host, $user, $db, $port, $socket);
}
$link->query('DROP USER shatest');
$link->query('DROP USER shatest@localhost');
?>
--EXPECTF--
done!
