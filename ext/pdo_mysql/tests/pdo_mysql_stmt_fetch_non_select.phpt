--TEST--
MySQL PDOStatement->execute()/fetch(), Non-SELECT
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();
	MySQLPDOTest::createTestTable($db);

	try {

		// Emulated PS first
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
		if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn on emulated prepared statements\n");

		if (!is_object($stmt = $db->query('DESCRIBE test id')))
			printf("[003] Emulated PS, DESCRIBE failed, %s\n", var_export($db->errorInfo(), true));

		$describe = array();
		$valid = false;
		while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
			$describe[] = $row;
			foreach ($row as $column => $value)
			if (isset($row['field']) && ($row['field'] == 'id'))
				$valid = true;
		}
		if (empty($describe))
			printf("[004] Emulated PS, DESCRIBE returned no results\n");
		else if (!$valid)
			printf("[005] Emulated PS, DESCRIBE, returned data seems wrong, dumping %s\n",
				var_export($describe, true));

		if (!is_object($stmt = $db->query('SHOW ENGINES')))
			printf("[006] Emulated PS, SHOW failed, %s\n", var_export($db->errorInfo(), true));

		$show = array();
		$valid = false;
		while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
			$show[] = $row;
			foreach ($row as $column => $value)
				// MyISAM engine should be part of _every_ MySQL today
				if ($value == 'MyISAM')
					$valid = true;
		}
		if (empty($show))
			printf("[007] Emulated PS, SHOW returned no results\n");
		else if (!$valid)
			printf("[008] Emulated PS, SHOW data seems wrong, dumping %s\n",
				var_export($show, true));

		if (!is_object($stmt = $db->query("EXPLAIN SELECT id FROM test")))
			printf("[009] Emulated PS, EXPLAIN returned no results\n");

		$explain = array();
		while ($row = $stmt->fetch(PDO::FETCH_ASSOC))
			$explain[] = $row;

		if (empty($explain))
			printf("[010] Emulated PS, EXPLAIN returned no results\n");

		// And now native PS
		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[011] Unable to turn off emulated prepared statements\n");

		$native_support = 'no';
		if ($db->exec("PREPARE mystmt FROM 'DESCRIBE test id'")) {
			$native_support = 'yes';
			$db->exec('DEALLOCATE PREPARE mystmt');
		}

		if (!is_object($stmt = $db->query('DESCRIBE test id')))
			printf("[012] Native PS (native support: %s), DESCRIBE failed, %s\n",
				$native_support,
				var_export($db->errorInfo(), true));

		$describe_native = array();
		$valid = false;
		while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
			$describe_native[] = $row;
			foreach ($row as $column => $value)
			if (isset($row['field']) && ($row['field'] == 'id'))
				$valid = true;
		}
		if (empty($describe_native))
			printf("[013] Native PS (native support: %s), DESCRIBE returned no results\n",
				$native_support);
		else if (!$valid)
			printf("[014] Native PS (native support: %s), DESCRIBE, returned data seems wrong, dumping %s\n",
				$native_support,
				var_export($describe_native, true));

		if ($describe != $describe_native)
			printf("[015] Emulated and native PS (native support: %s) results of DESCRIBE differ: %s vs. %s\n",
				$native_support,
				var_export($describe, true),
				var_export($describe_native, true));


		$native_support = 'no';
		if ($db->exec("PREPARE mystmt FROM 'SHOW ENGINES'")) {
			$native_support = 'yes';
			$db->exec('DEALLOCATE PREPARE mystmt');
		}

		if (!is_object($stmt = $db->query('SHOW ENGINES')))
			printf("[016] Native PS (native support: %s), SHOW failed, %s\n",
				$native_support,
				var_export($db->errorInfo(), true));

		$show_native = array();
		$valid = false;
		while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
			$show_native[] = $row;
			foreach ($row as $column => $value)
				// MyISAM engine should be part of _every_ MySQL today
				if ($value == 'MyISAM')
					$valid = true;
		}
		if (empty($show_native))
			printf("[017] Native PS (native support: %s), SHOW returned no results\n",
				$native_support);
		else if (!$valid)
			printf("[018] Native PS (native support: %s), SHOW data seems wrong, dumping %s\n",
				var_export($show_native, true));

		if ($show != $show_native)
			printf("Native PS (native support: %s) and emulated PS returned different data for SHOW: %s vs. %s\n",
				$native_support,
				var_export($show, true),
				var_export($show_native, true));

		$native_support = 'no';
		if ($db->exec("PREPARE mystmt FROM 'EXPLAIN SELECT id FROM test'")) {
			$native_support = 'yes';
			$db->exec('DEALLOCATE PREPARE mystmt');
		}

		if (!is_object($stmt = $db->query("EXPLAIN SELECT id FROM test")))
			printf("[012] Native PS (native support: %s), EXPLAIN failed, %s\n",
				$native_support,
				var_export($db->errorInfo(), true));

		$explain_native = array();
		while ($row = $stmt->fetch(PDO::FETCH_ASSOC))
			$explain_native[] = $row;

		if (empty($explain_native))
			printf("[013] Native PS (native support: %s), EXPLAIN returned no results\n",
				$native_support);

		if ($explain != $explain_native)
			printf("Native PS (native support: %s) and emulated PS returned different data for EXPLAIN: %s vs. %s\n",
				$native_support,
				var_export($explain, true),
				var_export($explain_native, true));

		$stmt->execute();
		$explain_native = $stmt->fetchAll(PDO::FETCH_ASSOC);
		if ($explain != $explain_native)
			printf("Native PS (native support: %s) and emulated PS returned different data for EXPLAIN: %s vs. %s\n",
				$native_support,
				var_export($explain, true),
				var_export($explain_native, true));

		$stmt->execute();
		$stmt->execute();
		// libmysql needs this - otherwise we get a 2015 error
		if (!MYSQLPDOTest::isPDOMySQLnd())
			$stmt->fetchAll(PDO::FETCH_ASSOC);

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!\n";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
done!
