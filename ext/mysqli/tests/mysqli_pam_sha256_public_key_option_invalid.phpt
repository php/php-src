--TEST--
PAM: SHA-256, option: MYSQLI_SERVER_PUBLIC_KEY (invalid)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
ob_start();
phpinfo(INFO_MODULES);
$tmp = ob_get_contents();
ob_end_clean();
if (!stristr($tmp, "auth_plugin_sha256_password"))
    die("skip SHA256 auth plugin not built-in to mysqlnd");

require_once 'connect.inc';
if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

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

if (!($res = $link->query("SHOW STATUS LIKE 'Rsa_public_key'"))) {
    die(sprintf("skip [%d] %s\n", $link->errno, $link->error));
}

if (!($row = $res->fetch_assoc())) {
    die(sprintf("skip Failed to check RSA pub key, [%d] %s\n", $link->errno, $link->error));
}

if (strlen($row['Value']) < 100) {
    die(sprintf("skip Server misconfiguration? RSA pub key is suspicious, [%d] %s\n", $link->errno, $link->error));
}

/* date changes may give false positive */
$file = sprintf("%s%s%s_%s", sys_get_temp_dir(), DIRECTORY_SEPARATOR, "test_sha256_" , @date("Ymd"));
if ((file_exists($file) && !unlink($file)) || !($fp = @fopen($file, "w"))) {
    die(sprintf("skip Cannot create RSA pub key file '%s'", $file));
}
if (strlen($row['Value']) != fwrite($fp, $row['Value'])) {
    die(sprintf("skip Failed to create pub key file"));
}

// Ignore errors because this variable exists only in MySQL 5.6 and 5.7
$link->query("SET @@session.old_passwords=2");

$link->query('DROP USER shatest');
$link->query("DROP USER shatest@localhost");


if (!$link->query('CREATE USER shatest@"%" IDENTIFIED WITH sha256_password') ||
    !$link->query('CREATE USER shatest@"localhost" IDENTIFIED WITH sha256_password')) {
    die(sprintf("skip CREATE USER failed [%d] %s", $link->errno, $link->error));
}

if (!$link->query('SET PASSWORD FOR shatest@"%" = "shatest"') ||
    !$link->query('SET PASSWORD FOR shatest@"localhost" = "shatest"')) {
    die(sprintf("skip SET PASSWORD failed [%d] %s", $link->errno, $link->error));
}

if (!$link->query("DROP TABLE IF EXISTS test") ||
    !$link->query("CREATE TABLE test (id INT)") ||
    !$link->query("INSERT INTO test(id) VALUES (1), (2), (3)"))
    die(sprintf("SKIP [%d] %s\n", $link->errno, $link->error));


if (!$link->query(sprintf("GRANT SELECT ON TABLE %s.test TO shatest@'%%'", $db)) ||
    !$link->query(sprintf("GRANT SELECT ON TABLE %s.test TO shatest@'localhost'", $db))) {
    die(sprintf("skip Cannot grant SELECT to user [%d] %s", mysqli_errno($link), mysqli_error($link)));
}

$link->close();
echo "nocache";
?>
--FILE--
<?php
    require_once "connect.inc";

    function sha_connect($offset, $host, $db, $port, $socket, $file) {

        $link = mysqli_init();
        if (!($link->options(MYSQLI_SERVER_PUBLIC_KEY, $file))) {
            printf("[%03d + 001] mysqli_options failed, [%d] %s\n", $offset, $link->errno, $link->error);
            return false;
        }

        if (!$link->real_connect($host, 'shatest', 'shatest', $db, $port, $socket)) {
            printf("[%03d + 002] [%d] %s\n", $offset, $link->connect_errno, $link->connect_error);
            return false;
        }

        if (!$res = $link->query("SELECT id FROM test WHERE id = 1"))
            printf("[%03d + 003] [%d] %s\n", $offset, $link->errno, $link->error);
            return false;

        if (!$row = mysqli_fetch_assoc($res)) {
            printf("[%03d + 004] [%d] %s\n", $offset, $link->errno, $link->error);
            return false;
        }

        if ($row['id'] != 1) {
            printf("[%03d + 005] Expecting 1 got %s/'%s'", $offset, gettype($row['id']), $row['id']);
            return false;
        }

        $res->close();
        $link->close();
        return true;
    }

    $file = sprintf("%s%s%s_%s", sys_get_temp_dir(), DIRECTORY_SEPARATOR, "test_sha256_" , @date("Ymd"));
    if (file_exists($file) && is_readable($file)) {

        /* valid key */
        sha_connect(100, $host, $db, $port, $socket, $file);

        /* invalid key */
        $file_wrong = sprintf("%s%s%s_%s", sys_get_temp_dir(), DIRECTORY_SEPARATOR, "test_sha256_wrong" , @date("Ymd"));

        $key = file_get_contents($file);
        $key = str_replace("A", "a", $key);
        $key = str_replace("M", "m", $key);
        @unlink($file_wrong);
        if (!($fp = fopen($file_wrong, "w"))) {
            printf("[002] Can't write public key file.");
        } else {
            fwrite($fp, $key);
            fclose($fp);
            sha_connect(200, $host, $db, $port, $socket, $file_wrong);
        }

        /* empty file */
        @unlink($file_wrong);
        if (!($fp = fopen($file_wrong, "w"))) {
            printf("[003] Can't write public key file.");
        } else {
            fwrite($fp, "");
            fclose($fp);
            sha_connect(300, $host, $db, $port, $socket, $file_wrong);
        }

        /* file does not exist */
        @unlink($file_wrong);
        sha_connect(400, $host, $db, $port, $socket, $file_wrong);

    } else {
        printf("[001] Cannot read public key file.");
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once "clean_table.inc";
    $link->query('DROP USER shatest');
    $link->query('DROP USER shatest@localhost');
    $file = sprintf("%s%s%s_%s", sys_get_temp_dir(), DIRECTORY_SEPARATOR, "test_sha256_" , @date("Ymd"));
    @unlink($file);
    $file_wrong = sprintf("%s%s%s_%s", sys_get_temp_dir(), DIRECTORY_SEPARATOR, "test_sha256_wrong" , @date("Ymd"));
    @unlink($file_wrong);
?>
--EXPECTF--
Warning: mysqli::real_connect(): (HY000/1045): %s in %s on line %d
[200 + 002] [1045] %s

Warning: mysqli::real_connect(): (HY000/1045): %s in %s on line %d
[300 + 002] [1045] %s

Warning: mysqli::real_connect(%sest_sha256_wrong_%d): Failed to open stream: No such file or directory in %s on line %d

Warning: mysqli::real_connect(): (HY000/1045): %s in %s on line %d
[400 + 002] [1045] %s
done!
