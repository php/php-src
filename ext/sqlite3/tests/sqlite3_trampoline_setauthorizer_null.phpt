--TEST--
SQLite3 user authorizer null
--EXTENSIONS--
sqlite3
--FILE--
<?php

$db = new SQLite3(':memory:');
$db->enableExceptions(true);

$db->setAuthorizer(null);

// This query should be accepted
var_dump($db->querySingle('SELECT 1;'));

try {
    // This one should fail
    var_dump($db->querySingle('CREATE TABLE test (a, b);'));
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
int(1)
NULL
