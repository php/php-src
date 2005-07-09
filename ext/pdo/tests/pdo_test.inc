<?php
# PDO test framework utilities

class PDOTest {
	// create an instance of the PDO driver, based on
	// the current environment
	static function factory($classname = 'PDO') {
		$dsn = getenv('PDOTEST_DSN');
		$user = getenv('PDOTEST_USER');
		$pass = getenv('PDOTEST_PASS');
		$attr = getenv('PDOTEST_ATTR');
		if (is_string($attr) && strlen($attr)) {
			$attr = unserialize($attr);
		} else {
			$attr = null;
		}

		if ($user === false) $user = NULL;
		if ($pass === false) $pass = NULL;
		
		$db = new $classname($dsn, $user, $pass, $attr);

		// clean up any crufty test tables we might have left behind
		// on a previous run
		static $test_tables = array(
			'test',
			'classtypes'
			);
		foreach ($test_tables as $table) {
			$db->exec("DROP TABLE $table");
		}
		
		$db->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_WARNING);
		$db->setAttribute(PDO_ATTR_CASE, PDO_CASE_LOWER);
		$db->setAttribute(PDO_ATTR_STRINGIFY_FETCHES, true);
		return $db;
	}

	static function skip() {
		try {
			$db = PDOTest::factory();
		} catch (PDOException $e) {
			die("skip " . $e->getMessage());
		}
	}
}



?>
