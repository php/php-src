--TEST--
Bug #61411 (PDO Segfaults with PERSISTENT == TRUE && EMULATE_PREPARES == FALSE)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();

$row = $db->query('SELECT VERSION() as _version')->fetch(PDO::FETCH_ASSOC);
$matches = array();
if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
	die(sprintf("skip Cannot determine MySQL Server version\n"));

$version = $matches[0] * 10000 + $matches[1] * 100 + $matches[2];
if ($version < 40106)
	die(sprintf("skip Need MySQL Server 4.1.6+, found %d.%02d.%02d (%d)\n",
		$matches[0], $matches[1], $matches[2], $version));
?>
--FILE--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$attr	= getenv('PDOTEST_ATTR');
if (!$attr) {
	$attr = array();
} else {
	$attr = unserialize($attr);
}
$attr[PDO::ATTR_PERSISTENT] = true;
$attr[PDO::ATTR_EMULATE_PREPARES] = false;
putenv('PDOTEST_ATTR='.serialize($attr));

$db = MySQLPDOTest::factory();

$stmt = $db->prepare("SELECT 1");
$stmt->execute();

foreach ($stmt as $line) {
	var_dump($line);
}

print "done!";
?>
--EXPECT--
array(2) {
  [1]=>
  int(1)
  [2]=>
  int(1)
}
done!
