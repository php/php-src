--TEST--
SQLite3 user authorizer callback
--EXTENSIONS--
sqlite3
--FILE--
<?php

$db = new SQLite3(':memory:');
$db->enableExceptions(true);

$db->setAuthorizer(function (int $action) {
    if ($action == SQLite3::SELECT) {
        return SQLite3::OK;
    }

    return SQLite3::DENY;
});

// This query should be accepted
var_dump($db->querySingle('SELECT 1;'));

try {
    // This one should fail
    var_dump($db->querySingle('CREATE TABLE test (a, b);'));
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

// Test disabling the authorizer
$db->setAuthorizer(null);

// This should now succeed
var_dump($db->exec('CREATE TABLE test (a); INSERT INTO test VALUES (42);'));
var_dump($db->querySingle('SELECT a FROM test;'));

// Test if we are getting the correct arguments
$db->setAuthorizer(function (int $action) {
    $constants = (new ReflectionClass('SQLite3'))->getConstants();
    $constants = array_flip($constants);

    var_dump($constants[$action], implode(',', array_slice(func_get_args(), 1)));
    return SQLITE3::OK;
});

var_dump($db->exec('SELECT * FROM test WHERE a = 42;'));
var_dump($db->exec('DROP TABLE test;'));

// Try to return something invalid from the authorizer
$db->setAuthorizer(function () {
    return 'FAIL';
});

try {
    var_dump($db->querySingle('SELECT 1;'));
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
    echo $e->getPrevious()->getMessage() . "\n";
}

$db->setAuthorizer(function () {
    return 4200;
});

try {
    var_dump($db->querySingle('SELECT 1;'));
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
    echo $e->getPrevious()->getMessage() . "\n";
}

?>
--EXPECT--
int(1)
Unable to prepare statement: 23, not authorized
bool(true)
int(42)
string(6) "SELECT"
string(3) ",,,"
string(4) "READ"
string(12) "test,a,main,"
string(4) "READ"
string(12) "test,a,main,"
bool(true)
string(6) "DELETE"
string(20) "sqlite_master,,main,"
string(10) "DROP_TABLE"
string(11) "test,,main,"
string(6) "DELETE"
string(11) "test,,main,"
string(6) "DELETE"
string(20) "sqlite_master,,main,"
string(4) "READ"
string(28) "sqlite_master,tbl_name,main,"
string(4) "READ"
string(24) "sqlite_master,type,main,"
string(6) "UPDATE"
string(28) "sqlite_master,rootpage,main,"
string(4) "READ"
string(28) "sqlite_master,rootpage,main,"
bool(true)
Unable to prepare statement: 23, not authorized
The authorizer callback returned an invalid type: expected int
Unable to prepare statement: 23, not authorized
The authorizer callback returned an invalid value
