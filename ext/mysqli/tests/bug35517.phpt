--TEST--
Bug #35517 (mysqli_stmt_fetch returns NULL)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, $db, $port, $socket);

	$mysql->query("CREATE TABLE temp (id INT UNSIGNED NOT NULL)");
	$mysql->query("INSERT INTO temp (id) VALUES (3000000897),(3800001532),(3900002281),(3100059612)");
	$stmt = $mysql->prepare("SELECT id FROM temp");
	$stmt->execute();
	$stmt->bind_result($id);
	while ($stmt->fetch()) {
		if (PHP_INT_SIZE == 8) {
			if ((gettype($id) !== 'int') && (gettype($id) != 'integer'))
				printf("[001] Expecting integer on 64bit got %s/%s\n", gettype($id), var_export($id, true));
		} else {
			if (gettype($id) !== 'string') {
				printf("[002] Expecting string on 32bit got %s/%s\n", gettype($id), var_export($id, true));
			}
			if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) && !is_unicode($id)) {
				printf("[003] Expecting unicode string\n");
			}
		}
		print $id;
		print "\n";
	}
	$stmt->close();

	$mysql->query("DROP TABLE temp");
	$mysql->close();
	print "done!";
?>
--EXPECTF--
3000000897
3800001532
3900002281
3100059612
done!