--TEST--
Bug #45798 (sqlite3 doesn't track unexecuted statements)
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$stmt = $db->prepare("SELECT 1");

var_dump($stmt->close());

var_dump($db->close());

print "done";

?>
--EXPECT--
bool(true)
bool(true)
done
