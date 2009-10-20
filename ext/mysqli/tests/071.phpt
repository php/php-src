--TEST--
mysqli thread_id & kill
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
	$version = $mysql->server_version;

	var_dump($mysql->ping());

	$ret = $mysql->kill($mysql->thread_id);
	if ($IS_MYSQLND) {
		if ($ret !== true){
			printf("[001] Expecting boolean/true got %s/%s\n", gettype($ret), var_export($ret, true));
		}
	} else {
		/* libmysql return value seems to depend on server version */
		if (($version >= 50123 || $version <= 40200) && $version != 50200) {
			/* TODO: find exact version */
			if ($ret !== true){
				printf("[001] Expecting boolean/true got %s/%s @\n", gettype($ret), var_export($ret, true), $version);
			}
		} else {
			if ($ret !== false){
			printf("[001] Expecting boolean/false got %s/%s @\n", gettype($ret), var_export($ret, true), $version);
			}
		}
	}

	var_dump($mysql->ping());

	$mysql->close();

	$mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

	var_dump(mysqli_ping($mysql));

	$ret = $mysql->kill($mysql->thread_id);
	if ($IS_MYSQLND) {
		if ($ret !== true){
			printf("[002] Expecting boolean/true got %s/%s\n", gettype($ret), var_export($ret, true));
		}
	} else {
		/* libmysql return value seems to depend on server version */
		if (($version >= 50123 || $version <= 40200) && $version != 50200) {
			/* TODO: find exact version */
			if ($ret !== true){
				printf("[002] Expecting boolean/true got %s/%s @\n", gettype($ret), var_export($ret, true), $version);
			}
		} else {
			if ($ret !== false){
			printf("[002] Expecting boolean/false got %s/%s @\n", gettype($ret), var_export($ret, true), $version);
			}
		}
	}

	var_dump(mysqli_ping($mysql));

	$mysql->close();
	print "done!";
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
done!