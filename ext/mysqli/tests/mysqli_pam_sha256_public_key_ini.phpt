--TEST--
PAM: SHA-256, mysqlnd.sha256_server_public_key
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

ob_start();
phpinfo(INFO_MODULES);
$tmp = ob_get_contents();
ob_end_clean();
if (!stristr($tmp, "auth_plugin_sha256_password"))
	die("skip SHA256 auth plugin not built-in to mysqlnd");

require_once('connect.inc');
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

if (!($res = $link->query("SHOW STATUS LIKE 'Rsa_public_key'"))) {
	die(sprintf("skip [%d] %s\n", $link->errno, $link->error));
}

if (!($row = $res->fetch_assoc())) {
	die(sprintf("skip Failed to check RSA pub key, [%d] %s\n", $link->errno, $link->error));
}

$key = $row['Value'];
if (strlen($key) < 100) {
	die(sprintf("skip Server misconfiguration? RSA pub key is suspicious, [%d] %s\n", $link->errno, $link->error));
}

/* date changes may give false positive */
$file = "test_sha256_ini";
if ((file_exists($file) && !unlink($file)) || !($fp = @fopen($file, "w"))) {
	die(sprintf("skip Cannot create RSA pub key file '%s'", $file));
}
$key = str_replace("A", "a", $key);
$key = str_replace("M", "m", $key);
if (strlen($key) != fwrite($fp, $key)) {
	die(sprintf("skip Failed to create pub key file"));
}


if (!$link->query("SET @@session.old_passwords=2")) {
	die(sprintf("skip Cannot set @@session.old_passwords=2 [%d] %s", $link->errno, $link->error));
}

$link->query('DROP USER shatest');
$link->query("DROP USER shatest@localhost");


if (!$link->query('CREATE USER shatest@"%" IDENTIFIED WITH sha256_password') ||
	!$link->query('CREATE USER shatest@"localhost" IDENTIFIED WITH sha256_password')) {
	die(sprintf("skip CREATE USER failed [%d] %s", $link->errno, $link->error));
}

if (!$link->query('SET PASSWORD FOR shatest@"%" = PASSWORD("shatest")') ||
	!$link->query('SET PASSWORD FOR shatest@"localhost" = PASSWORD("shatest")')) {
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
?>
--INI--
mysqlnd.sha256_server_public_key="test_sha256_ini"
--FILE--
<?php
	require_once("connect.inc");


	$link = new mysqli($host, 'shatest', 'shatest', $db, $port, $socket);
	if ($link->connect_errno) {
		printf("[001] [%d] %s\n", $link->connect_errno, $link->connect_error);
	} else {
		if (!$res = $link->query("SELECT id FROM test WHERE id = 1"))
			printf("[002] [%d] %s\n", $link->errno, $link->error);

		if (!$row = mysqli_fetch_assoc($res)) {
			printf("[003] [%d] %s\n", $link->errno, $link->error);
		}

		if ($row['id'] != 1) {
			printf("[004] Expecting 1 got %s/'%s'", gettype($row['id']), $row['id']);
		}
	}
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
	$link->query('DROP USER shatest');
	$link->query('DROP USER shatest@localhost');
	$file = "test_sha256_ini";
	@unlink($file);
?>
--EXPECTF--

Warning: mysqli::__construct(): (HY000/1045): %s in %s on line %d
[001] [1045] %s
done!