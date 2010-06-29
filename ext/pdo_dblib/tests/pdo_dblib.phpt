--TEST--
PDO_DBLIB: Core tests
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

test_transaction();
test_quotedident();
test_prepare();

function test_quotedident() {
	global $db;
	$db->query('CREATE TABLE "Test Table" ("My Field" int, "Another Field" varchar(32) not null default \'test_string\')');
	$db->query('INSERT INTO "Test Table" ("My Field") values(1)');
	$db->query('INSERT INTO "Test Table" ("My Field") values(2)');
	$db->query('INSERT INTO "Test Table" ("My Field") values(3)');
	$rs = $db->query('SELECT * FROM "Test Table"');
	var_dump($rs->fetchAll(PDO::FETCH_ASSOC));
	$db->query('DROP TABLE "Test Table"');
}

function test_transaction() {
	global $db;
	echo "Transactions\n";

	/*We see these rows */
	$db->query("CREATE table php_test(val int)");
	$db->beginTransaction();
	$db->query("INSERT INTO php_test(val) values(1)");
	$db->query("INSERT INTO php_test(val) values(2)");
	$db->query("INSERT INTO php_test(val) values(3)");
	$db->query("INSERT INTO php_test(val) values(4)");
	$db->commit();

	/*We don't see these rows */
	$db->beginTransaction();
	$db->query("INSERT INTO php_test(val) values(5)");
	$db->query("INSERT INTO php_test(val) values(6)");
	$db->query("INSERT INTO php_test(val) values(7)");
	$db->query("INSERT INTO php_test(val) values(8)");
	$db->rollback();

	$rs = $db->query("SELECT * FROM php_test");
	$rows = $rs->fetchAll(PDO::FETCH_ASSOC);
	var_dump($rows);

	$db->query("DROP table php_test");

}

$db = null;

function test_prepare() {
	global $db;
	echo "Prepare:\n";
	$stmt = $db->prepare("select ic1.*
	from information_schema.columns ic1
	cross join information_schema.columns ic2
	cross join information_schema.columns ic3");
	
	echo "Execute:\n";
	$x = $stmt->execute();

	echo "Fetch\n";
	$n = 0;
	$mem = 0;
	while (($r = $stmt->fetch())) {
		if ($n ==0)
			$mem = memory_get_usage();
		if ($n % 1000 == 0) {
			echo($mem - memory_get_usage());
		}
		$n++;
		
	}
	$stmt = null;
}

echo "All done\n";
?>
--EXPECT--
