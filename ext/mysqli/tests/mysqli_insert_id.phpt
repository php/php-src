--TEST--
mysqli_insert_id()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_insert_id()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_insert_id($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (0 !== ($tmp = mysqli_insert_id($link)))
		printf("[003] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

	if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id LIMIT 1")) {
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	if (0 !== ($tmp = mysqli_insert_id($link)))
		printf("[005] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);
	mysqli_free_result($res);

	// no auto_increment column
	if (!$res = mysqli_query($link, "INSERT INTO test(id, label) VALUES (100, 'a')")) {
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	if (0 !== ($tmp = mysqli_insert_id($link)))
		printf("[007] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

	if (!$res = mysqli_query($link, "ALTER TABLE test MODIFY id INT NOT NULL AUTO_INCREMENT")) {
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	if (!$res = mysqli_query($link, "INSERT INTO test(label) VALUES ('a')")) {
		printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	if (($last_id = mysqli_insert_id($link)) <= 0)
		printf("[010] Expecting int/any >0, got %s/%s\n", gettype($last_id), $last_id);

	if (mysqli_query($link, "LOCK TABLE test WRITE")) {
		/* we need exclusive access for a moment */
		/* let's hope nobody changes auto_increment_increment while this code executes */
		do {
			if (mysqli_get_server_version($link) >= 50000) {
				if (!$res = mysqli_query($link, 'SELECT @@auto_increment_increment AS inc')) {
					printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
					break;
				}
				if (!$row = mysqli_fetch_assoc($res)) {
					printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
					break;
				}
				mysqli_free_result($res);
				$inc = $row['inc'];
			} else {
				$inc = 1;
			}

			if (!mysqli_query($link, "INSERT INTO test(label) VALUES ('b')")) {
				printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
				break;
			}
			if (($next_id = mysqli_insert_id($link)) <= $last_id)
				/*
				very likely a bug, but someone could have done something on the server
				between the second last insert and the lock, therefore don't stop just bail
				*/
				printf("[014] Expecting int/any > %d, got %s/%s\n", $last_id, gettype($next_id), $next_id);

			$last_id = $next_id;
			if (!mysqli_query($link, "INSERT INTO test(label) VALUES ('c'), ('d'), ('e')")) {
				printf("[015] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
				break;
			}
			/*
			Note: For a multiple-row insert, LAST_INSERT_ID() and mysql_insert_id() actually
			return the AUTO_INCREMENT key from the first of the inserted rows. This allows
			multiple-row inserts to be reproduced correctly on other servers in a replication setup.
			*/
			if (($next_id = mysqli_insert_id($link)) != $last_id + $inc) {
				printf("[016] Expecting int/%d, got %s/%s\n", $last_id + 1, gettype($next_id), $next_id);
				break;
			}

			if (!$res = mysqli_query($link, "SELECT LAST_INSERT_ID() AS last_id")) {
				printf("[017] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
				break;
			}
			if (!$row = mysqli_fetch_assoc($res)) {
				printf("[018] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
				break;
			}
			mysqli_free_result($res);

			if ($next_id != $row['last_id']) {
				printf("[019] Something is wrong, check manually. Expecting %s got %s.\n",
					$next_id, $row['last_id']);
				break;
			}
		} while (false);
		mysqli_query($link, "UNLOCK TABLE test");
	}

	if (!$res = mysqli_query($link, "INSERT INTO test(id, label) VALUES (1000, 'a')")) {
		printf("[020] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	if (1000 !== ($tmp = mysqli_insert_id($link)))
		printf("[021] Expecting int/1000, got %s/%s\n", gettype($tmp), $tmp);

	if (!$res = mysqli_query($link, "INSERT INTO test(label) VALUES ('b'), ('c')")) {
		printf("[022] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	if (1000 >= ($tmp = mysqli_insert_id($link)))
		printf("[023] Expecting int/>1000, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	var_dump(mysqli_insert_id($link));

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_insert_id(): Couldn't fetch mysqli in %s on line %d
NULL
done!