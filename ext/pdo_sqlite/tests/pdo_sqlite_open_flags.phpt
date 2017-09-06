--TEST--
PDO_sqlite: Testing open flags
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php

$filename = tempnam(sys_get_temp_dir(), 'sqlite');

// Default open flag is read-write|create
$db = new PDO('sqlite:' . $filename, null, null, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);

var_dump($db->exec('CREATE TABLE test1 (id INT);'));

try {
	$db = new PDO('sqlite:' . $filename, null, null, [PDO::SQLITE_ATTR_OPEN_FLAGS => PDO::SQLITE_OPEN_READONLY, PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);

	var_dump($db->exec('CREATE TABLE test2 (id INT);'));
}
finally {
	// Cleanup
	unlink($filename);
}

?>
--EXPECTF--
int(0)

Fatal error: Uncaught PDOException: SQLSTATE[HY000]: General error: 8 attempt to write a readonly database in %s
Stack trace:
%s
#1 {main}
  thrown in %s
