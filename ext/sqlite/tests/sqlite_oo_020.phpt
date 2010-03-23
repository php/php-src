--TEST--
sqlite-oo: factory and exception
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
$dbname = tempnam(dirname(__FILE__), "phpsql");
function cleanup() {
	global $db, $dbname;

	$db = NULL;
	unlink($dbname);
}
register_shutdown_function("cleanup");

try {
	$db = sqlite_factory();
} catch(SQLiteException $err) {
	echo "Message: ".$err->getMessage()."\n";
	echo "File: ".$err->getFile()."\n";
	//echo "Line: ".$err->getLine()."\n";
	//print_r($err->getTrace());
	//echo "BackTrace: ".$err->getTraceAsString()."\n";
}

$db = sqlite_factory($dbname);

$data = array(
	array (0 => 'one', 1 => 'two'),
	array (0 => 'three', 1 => 'four')
	);

$db->query("CREATE TABLE strings(a VARCHAR, b VARCHAR)");

foreach ($data as $str) {
	$db->query("INSERT INTO strings VALUES('${str[0]}','${str[1]}')");
}

$r = $db->unbufferedQuery("SELECT a, b from strings");
while ($r->valid()) {
	var_dump($r->current(SQLITE_NUM));
	$r->next();
}
$r = null;
$db = null;
echo "DONE!\n";
?>
--EXPECTF--
Message: sqlite_factory() expects at least 1 parameter, 0 given
File: %ssqlite_oo_020.php
array(2) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
}
array(2) {
  [0]=>
  string(5) "three"
  [1]=>
  string(4) "four"
}
DONE!
