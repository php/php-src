--TEST--
mysqli_fetch_array() - large packages (to test compression)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";

	function mysqli_fetch_array_large($offset, $link, $package_size) {

		/* we are aiming for maximum compression to test MYSQLI_CLIENT_COMPRESS */
		$random_char = str_repeat('a', 255);
		$sql = "INSERT INTO test(label) VALUES ";

		while (strlen($sql) < ($package_size - 259))
			$sql .= sprintf("('%s'), ", $random_char);

		$sql = substr($sql, 0, -2);
		assert(strlen($sql) < $package_size);

		if (!mysqli_query($link, $sql)) {
			printf("[%03d + 1] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		/* buffered result set - let's hope we do not run into PHP memory limit... */
		if (!$res = mysqli_query($link, "SELECT id, label FROM test")) {
			printf("[%03d + 2] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		while ($row = mysqli_fetch_assoc($res)) {
			if ($row['label'] != $random_char) {
				printf("[%03d + 3] Wrong results - expecting '%s' got '%s', [%d] %s\n",
					$offset, $random_char, $row['label'], mysqli_errno($link), mysqli_error($link));
				return false;
			}
		}
		mysqli_free_result($res);

		if (!$stmt = mysqli_prepare($link, "SELECT id, label FROM test")) {
			printf("[%03d + 4] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		/* unbuffered result set */
		if (!mysqli_stmt_execute($stmt)) {
			printf("[%03d + 5] [%d] %s\n", $offset, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		$id = $label = NULL;
		if (!mysqli_stmt_bind_result($stmt, $id, $label)) {
			printf("[%03d + 6] [%d] %s\n", $offset, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		while (mysqli_stmt_fetch($stmt)) {
			if ($label != $random_char) {
				printf("[%03d + 7] Wrong results - expecting '%s' got '%s', [%d] %s\n",
					$offset, $random_char, $label, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
				return false;
			}
		}

		mysqli_stmt_free_result($stmt);
		mysqli_stmt_close($stmt);

		return true;
	}


	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);
	}

	if (!mysqli_query($link, "DROP TABLE IF EXISTS test") ||
			!mysqli_query($link, sprintf("CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, label VARCHAR(255)) ENGINE = %s", $engine)))
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$package_size = 32768;
	$offset = 3;
	$limit = (0 < ini_get('memory_limit')) ? pow(2, 32) : ini_get('memory_limit');
	while (($package_size < $limit) && mysqli_fetch_array_large($offset++, $link, $package_size))
		$package_size += $package_size;

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
[%d + 1] [1153] Got a packet bigger than 'max_allowed_packet' bytes
done!