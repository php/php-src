<?php
	/* Utility function for mysqli_set_local_infile*.phpt tests */
	function shutdown_clean($file) {
		if ($file) {
			unlink($file);
		}
	}

	function check_local_infile_support($link, $engine, $table_name = 'test') {

		if (!$res = mysqli_query($link, 'SHOW VARIABLES LIKE "local_infile"'))
			return "Cannot check if Server variable 'local_infile' is set to 'ON'";

		$row = mysqli_fetch_assoc($res);
		mysqli_free_result($res);
		if ('ON' != $row['Value'])
			return sprintf("Server variable 'local_infile' seems not set to 'ON', found '%s'", $row['Value']);

		if (!mysqli_query($link, sprintf('DROP TABLE IF EXISTS %s', $table_name))) {
			return "Failed to drop old test table";
		}

		if (!mysqli_query($link, $sql = sprintf('CREATE TABLE %s(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE=%s',
			$table_name, $engine)))
			return "Failed to create test table: $sql";

		$file = create_standard_csv(1, false);
		if (!$file) {
			mysqli_query($link, sprintf('DROP TABLE IF EXISTS %s', $table_name));
			return "Cannot create CSV file";
		}

		if (!@mysqli_query($link, sprintf("LOAD DATA LOCAL INFILE '%s'
			INTO TABLE %s
			FIELDS TERMINATED BY ';' OPTIONALLY ENCLOSED BY '\''
			LINES TERMINATED BY '\n'",
			mysqli_real_escape_string($link, $file),
			$table_name))) {
			  if (1148 == mysqli_errno($link)) {
				  mysqli_query($link, sprintf('DROP TABLE IF EXISTS %s', $table_name));
				  return "Cannot test LOAD DATA LOCAL INFILE, [1148] The used command is not allowed with this MySQL version";
			  } else if ($link->errno) {
				  return $link->error;
			  }
		}
		mysqli_query($link, sprintf('DROP TABLE IF EXISTS %s', $table_name));
		return "";
	}

	function create_standard_csv($offset, $verbose = true) {
		// create a CVS file
		$file = tempnam(sys_get_temp_dir(), 'mysqli_test');
		if (!$fp = fopen($file, 'w')) {
			if ($verbose)
				printf("[%03d + 1] Cannot create CVS file '%s'\n", $offset, $file);
			return NULL;
		} else {
			/* Looks ugly? No, handy if you have crashes... */
			register_shutdown_function("shutdown_clean", $file);
		}

		if (!fwrite($fp, "97;'x';\n") ||
			!fwrite($fp, "98;'y';\n") ||
			!fwrite($fp, "99;'z';\n")) {
			if ($verbose)
				printf("[%03d + 3] Cannot write CVS file '%s'\n", $offset, $file);
			return NULL;
		}

		fclose($fp);

		if (!chmod($file, 0644)) {
			if ($verbose)
				printf("[%03d + 4] Cannot change the file perms of '%s' from 0600 to 0644, MySQL might not be able to read it\n",
					$offset, $file);
			return NULL;
		}
		return $file;
	}

	function try_handler($offset, $link, $file, $handler, $expected = null) {

		if ('default' == $handler) {
			mysqli_set_local_infile_default($link);
		} else if (!mysqli_set_local_infile_handler($link, $handler)) {
			printf("[%03d] Cannot set infile handler to '%s'\n", $offset, $handler);
			return false;
		}
		printf("Callback set to '%s'\n", $handler);

		if (!mysqli_query($link, sprintf("DELETE FROM test"))) {
			printf("[%03d] Cannot remove records, [%d] %s\n", $offset + 1, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		if (!@mysqli_query($link, sprintf("LOAD DATA LOCAL INFILE '%s'
			INTO TABLE test
			FIELDS TERMINATED BY ';' OPTIONALLY ENCLOSED BY '\''
			LINES TERMINATED BY '\n'",
			mysqli_real_escape_string($link, $file)))) {
			printf("[%03d] LOAD DATA failed, [%d] %s\n",
				$offset + 2,
				mysqli_errno($link), mysqli_error($link));
		}

		if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id")) {
			printf("[%03d] [%d] %s\n", $offset + 3, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		if (!is_array($expected))
			return true;

		foreach ($expected as $k => $values) {
			if (!$tmp = mysqli_fetch_assoc($res)) {
				printf("[%03d/%d] [%d] '%s'\n", $offset + 4, $k, mysqli_errno($link), mysqli_error($link));
				return false;
			}
			if ($values['id'] != $tmp['id']) {
				printf("[%03d/%d] Expecting %s got %s\n",
					$offset + 5, $k,
					$values['id'], $tmp['id']);
					return false;
			}
			if ($values['label'] != $tmp['label']) {
				printf("[%03d/%d] Expecting %s got %s\n",
					$offset + 6, $k,
					$values['label'], $tmp['label']);
					return false;
			}
		}

		if ($res && $tmp = mysqli_fetch_assoc($res)) {
			printf("[%03d] More results than expected!\n", $offset + 7);
			do {
				var_dump($tmp);
			} while ($tmp = mysqli_fetch_assoc($res));
			return false;
		}

		if ($res)
			mysqli_free_result($res);

		return true;
	}
?>
