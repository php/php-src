--TEST--
PDO_OCI: PDOStatement->getColumnMeta
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(__DIR__ . '/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

echo "Preparations before the test\n";

require(__DIR__ . '/../../pdo/tests/pdo_test.inc');
try {
	$db = PDOTest::factory();
	$db->exec(<<<SQL
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE test';
EXCEPTION
   WHEN OTHERS THEN
      IF SQLCODE != -942 THEN
	 RAISE;
      END IF;
END;
SQL
);
	$db->exec("CREATE TABLE test(id INT)");

	$db->beginTransaction();

	$stmt = $db->prepare('SELECT id FROM test ORDER BY id ASC');

	echo "Test 1. calling function with invalid parameters\n";

	// execute() has not been called yet
	// NOTE: no warning
	if (false !== ($tmp = $stmt->getColumnMeta(0)))
		printf("[002] Expecting false got %s\n", var_export($tmp, true));

	$stmt->execute();
	// Warning: PDOStatement::getColumnMeta() expects exactly 1 parameter, 0 given in
	if (false !== ($tmp = @$stmt->getColumnMeta()))
		printf("[003] Expecting false got %s\n", var_export($tmp, true));

	// invalid offset
	if (false !== ($tmp = @$stmt->getColumnMeta(-1)))
		printf("[004] Expecting false got %s\n", var_export($tmp, true));

	// Warning: PDOStatement::getColumnMeta() expects parameter 1 to be int, array given in
	if (false !== ($tmp = @$stmt->getColumnMeta(array())))
		printf("[005] Expecting false got %s\n", var_export($tmp, true));

	// Warning: PDOStatement::getColumnMeta() expects exactly 1 parameter, 2 given in
	if (false !== ($tmp = @$stmt->getColumnMeta(1, 1)))
		printf("[006] Expecting false got %s\n", var_export($tmp, true));

	// invalid offset
	if (false !== ($tmp = $stmt->getColumnMeta(1)))
		printf("[007] Expecting false because of invalid offset got %s\n", var_export($tmp, true));

	echo "Test 2. testing return values\n";
	echo "Test 2.1 testing array returned\n";

	$stmt = $db->prepare('SELECT id FROM test ORDER BY id ASC');
	$stmt->execute();
	$native = $stmt->getColumnMeta(0);
	if (count($native) == 0) {
		printf("[008] Meta data seems wrong, %s / %s\n",
			var_export($native, true), var_export($emulated, true));
	}


	function test_return($meta, $offset, $native_type, $pdo_type){
		if (empty($meta)) {
			printf("[%03d + 2] getColumnMeta() failed, %d - %s\n", $offset,
				$stmt->errorCode(), var_export($stmt->errorInfo(), true));
			return false;
		}
		$elements = array('flags', 'scale', 'name', 'len', 'precision', 'pdo_type');
		foreach ($elements as $k => $element)
			if (!isset($meta[$element])) {
				printf("[%03d + 3] Element %s missing, %s\n", $offset,
					$element, var_export($meta, true));
				return false;
			}

		if (!is_null($native_type)) {
			if (!isset($meta['native_type'])) {
				printf("[%03d + 4] Element native_type missing, %s\n", $offset,
					var_export($meta, true));
				return false;
			}

			if (!is_array($native_type))
				$native_type = array($native_type);

			$found = false;
			foreach ($native_type as $k => $type) {
				if ($meta['native_type'] == $type) {
					$found = true;
					break;
				}
			}

			if (!$found) {
				printf("[%03d + 5] Expecting native type %s, %s\n", $offset,
					var_export($native_type, true), var_export($meta, true));
				return false;
			}
		}

		if (!is_null($pdo_type) && ($meta['pdo_type'] != $pdo_type)) {
			printf("[%03d + 6] Expecting PDO type %s got %s (%s)\n", $offset,
				$pdo_type, var_export($meta, true), var_export($meta['native_type']));
			return false;
		}

		return true;
	}


	function test_meta(&$db, $offset, $sql_type, $value, $native_type, $pdo_type) {

		$db->exec(<<<SQL
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE test';
EXCEPTION
   WHEN OTHERS THEN
      IF SQLCODE != -942 THEN
         RAISE;
      END IF;
END;
SQL
);

		$sql = sprintf('CREATE TABLE test(id INT, label %s)', $sql_type);
		$stmt = $db->prepare($sql);
		$stmt->execute();

		if (!$db->exec(sprintf("INSERT INTO test(id, label) VALUES (1, '%s')", $value))) {
			printf("[%03d] + 1] Insert failed, %d - %s\n", $offset,
				$db->errorCode(), var_export($db->errorInfo(), true));
			return false;
		}

		$stmt = $db->prepare('SELECT id, label FROM test');
		$stmt->execute();
		$meta = $stmt->getColumnMeta(1);
		return test_return($meta, $offset, $native_type, $pdo_type);
	}

	echo "Test 2.2 testing numeric columns\n";

	test_meta($db, 20, 'NUMBER'         , 0                    , 'NUMBER', PDO::PARAM_STR);
	test_meta($db, 30, 'NUMBER'         , 256                  , 'NUMBER', PDO::PARAM_STR);
	test_meta($db, 40, 'INT'            , 256                  , 'NUMBER', PDO::PARAM_STR);
	test_meta($db, 50, 'INTEGER'        , 256                  , 'NUMBER', PDO::PARAM_STR);
	test_meta($db, 60, 'NUMBER'         , 256.01               , 'NUMBER', PDO::PARAM_STR);
	test_meta($db, 70, 'NUMBER'         , -8388608             , 'NUMBER', PDO::PARAM_STR);

	test_meta($db, 80, 'NUMBER'         , 2147483648           , 'NUMBER', PDO::PARAM_STR);
	test_meta($db, 90, 'NUMBER'         , 4294967295           , 'NUMBER', PDO::PARAM_STR);

	test_meta($db, 100, 'DEC'             , 1.01               , 'NUMBER'       , PDO::PARAM_STR);
	test_meta($db, 110, 'DECIMAL'         , 1.01               , 'NUMBER'       , PDO::PARAM_STR);
	test_meta($db, 120, 'FLOAT'           , 1.01               , 'FLOAT'        , PDO::PARAM_STR);
	test_meta($db, 130, 'DOUBLE PRECISION', 1.01               , 'FLOAT'        , PDO::PARAM_STR);
	test_meta($db, 140, 'BINARY_FLOAT'    , 1.01               , 'BINARY_FLOAT' , PDO::PARAM_STR);
	test_meta($db, 150, 'BINARY_DOUBLE'   , 1.01               , 'BINARY_DOUBLE', PDO::PARAM_STR);

	echo "Test 2.3 testing temporal columns\n";

	$db->exec("alter session set nls_date_format='YYYY-MM-DD'");
	test_meta($db, 160, 'DATE'           , '2008-04-23'        , 'DATE', PDO::PARAM_STR);

	echo "Test 2.4 testing string columns\n";

	test_meta($db, 170, 'CHAR(1)'       , 'a'                  , 'CHAR'     , PDO::PARAM_STR);
	test_meta($db, 180, 'CHAR(10)'      , '0123456789'         , 'CHAR'     , PDO::PARAM_STR);
	test_meta($db, 190, 'CHAR(255)'     , str_repeat('z', 255) , 'CHAR'     , PDO::PARAM_STR);
	test_meta($db, 200, 'VARCHAR(1)'    , 'a'                  , 'VARCHAR2' , PDO::PARAM_STR);
	test_meta($db, 210, 'VARCHAR(10)'   , '0123456789'         , 'VARCHAR2' , PDO::PARAM_STR);
	test_meta($db, 220, 'VARCHAR(255)'  , str_repeat('z', 255) , 'VARCHAR2' , PDO::PARAM_STR);
	test_meta($db, 230, 'VARCHAR2(1)'   , 'a'                  , 'VARCHAR2' , PDO::PARAM_STR);
	test_meta($db, 240, 'VARCHAR2(10)'  , '0123456789'         , 'VARCHAR2' , PDO::PARAM_STR);
	test_meta($db, 250, 'VARCHAR2(255)' , str_repeat('z', 255) , 'VARCHAR2' , PDO::PARAM_STR);

	test_meta($db, 260, 'NCHAR(1)'      , 'a'                  , 'NCHAR'    , PDO::PARAM_STR);
	test_meta($db, 270, 'NCHAR(10)'     , '0123456789'         , 'NCHAR'    , PDO::PARAM_STR);
	test_meta($db, 280, 'NCHAR(255)'    , str_repeat('z', 255) , 'NCHAR'    , PDO::PARAM_STR);
	test_meta($db, 290, 'NVARCHAR2(1)'  , 'a'                  , 'NVARCHAR2', PDO::PARAM_STR);
	test_meta($db, 300, 'NVARCHAR2(10)' , '0123456789'         , 'NVARCHAR2', PDO::PARAM_STR);
	test_meta($db, 310, 'NVARCHAR2(255)', str_repeat('z', 255) , 'NVARCHAR2', PDO::PARAM_STR);

	echo "Test 2.5 testing lobs columns\n";

	test_meta($db, 320, 'CLOB'          , str_repeat('b', 255) , 'CLOB'    , PDO::PARAM_LOB);
	test_meta($db, 330, 'BLOB'          , str_repeat('b', 256) , 'BLOB'    , PDO::PARAM_LOB);
	test_meta($db, 340, 'NCLOB'         , str_repeat('b', 255) , 'NCLOB'   , PDO::PARAM_LOB);

	test_meta($db, 350, 'LONG'          , str_repeat('b', 256) , 'LONG'    , PDO::PARAM_STR);
	test_meta($db, 360, 'LONG RAW'      , str_repeat('b', 256) , 'LONG RAW', PDO::PARAM_STR);
	test_meta($db, 370, 'RAW(256)'      , str_repeat('b', 256) , 'RAW'     , PDO::PARAM_STR);


	$db->exec(<<<SQL
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE test';
EXCEPTION
   WHEN OTHERS THEN
      IF SQLCODE != -942 THEN
         RAISE;
      END IF;
END;
SQL
);
	echo "Test 2.6 testing function return\n";

	$stmt = $db->query('SELECT count(*) FROM dual');
	$meta = $stmt->getColumnMeta(0);
	test_return($meta, 380, 'NUMBER', PDO::PARAM_STR);
	$stmt = $db->query("SELECT TO_DATE('2008-04-23') FROM dual");
	$meta = $stmt->getColumnMeta(0);
	test_return($meta, 390, 'DATE', PDO::PARAM_STR);
	$stmt = $db->query("SELECT TO_CHAR(542) FROM dual");
	$meta = $stmt->getColumnMeta(0);
	test_return($meta, 400, 'VARCHAR2', PDO::PARAM_STR);


	echo "Test 2.7 testing flags returned\n";

	$sql = sprintf('CREATE TABLE test(id INT NOT NULL, label INT NULL)');
	$stmt = $db->prepare($sql);
	$stmt->execute();
	$db->exec('INSERT INTO test(id, label) VALUES (1, 1)');
	$stmt = $db->query('SELECT id, label FROM test');
	$meta = $stmt->getColumnMeta(0);
	// verify the flags array countains a not_null flag and not nullable flags
	if (!isset($meta['flags'])) {
		printf("[1001] No flags contained in metadata %s\n", var_export($meta, true));
	} else {
		$flags = $meta['flags'];
		$found = false;
		foreach ($flags as $k => $flag) {
			if ($flag == 'not_null')
				$found = true;
			if ($flag == 'nullable')
				printf("[1003] Flags seem wrong %s\n", var_export($meta, true));
		}
		if (!$found)
			printf("[1002] Flags seem wrong %s\n", var_export($meta, true));
	}
	$meta = $stmt->getColumnMeta(1);
	// verify the flags array countains a nullable flag and not not_null flags
	if (!isset($meta['flags'])) {
		printf("[1003] No flags contained in metadata %s\n", var_export($meta, true));
	} else {
		$flags = $meta['flags'];
		$found = false;
		foreach ($flags as $k => $flag) {
			if ($flag == 'not_null')
				printf("[1004] Flags seem wrong %s\n", var_export($meta, true));
			if ($flag == 'nullable')
				$found = true;
		}
		if (!$found)
			printf("[1005] Flags seem wrong %s\n", var_export($meta, true));
	}

} catch (PDOException $e) {
	// we should never get here, we use warnings, but never trust a system...
	printf("[001] %s, [%s} %s\n",
		$e->getMessage(), $db->errorInfo(), implode(' ', $db->errorInfo()));
}

$db->exec(<<<SQL
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE test';
EXCEPTION
   WHEN OTHERS THEN
      IF SQLCODE != -942 THEN
         RAISE;
      END IF;
END;
SQL
);
print "done!";
?>
--EXPECT--
Preparations before the test
Test 1. calling function with invalid parameters
Test 2. testing return values
Test 2.1 testing array returned
Test 2.2 testing numeric columns
Test 2.3 testing temporal columns
Test 2.4 testing string columns
Test 2.5 testing lobs columns
Test 2.6 testing function return
Test 2.7 testing flags returned
done!
