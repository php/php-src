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
} catch(sqlite_exception $err) {
	echo "Message: ".$err->getmessage()."\n";
	echo "File: ".$err->getfile()."\n";
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

$r = $db->unbuffered_query("SELECT a, b from strings");
while ($r->has_more()) {
	var_dump($r->current(SQLITE_NUM));
	$r->next();
}
echo "DONE!\n";
?>
--EXPECTF--
Message: sqlite_factory() expects at least 1 parameter, 0 given
File: %s/sqlite_oo_020.php
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
