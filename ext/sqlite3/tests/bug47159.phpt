--TEST--
Bug #45798 (sqlite3 doesn't track unexecuted statements)
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/new_db.inc');

$stmt = $db->prepare("SELECT 1");

var_dump($stmt->close());

var_dump($db->close());

print "done";

?>
--EXPECT--
bool(true)
bool(true)
done
