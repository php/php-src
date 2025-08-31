--TEST--
Pdo\Sqlite user authorizer callback
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new Pdo\Sqlite('sqlite::memory:');

$db->setAuthorizer(function (int $action) {
    if ($action == 21 /* SELECT */) {
        return Pdo\Sqlite::OK;
    }

    return Pdo\Sqlite::DENY;
});

// This query should be accepted
var_dump($db->query('SELECT 1;'));

try {
    // This one should fail
    var_dump($db->exec('CREATE TABLE test (a, b);'));
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

// Test disabling the authorizer
$db->setAuthorizer(null);

// This should now succeed
var_dump($db->exec('CREATE TABLE test (a); INSERT INTO test VALUES (42);'));
var_dump($db->exec('SELECT a FROM test;'));

// Test if we are getting the correct arguments
$db->setAuthorizer(function (int $action) {
    $constants = ["COPY", "CREATE_INDEX", "CREATE_TABLE", "CREATE_TEMP_INDEX", "CREATE_TEMP_TABLE", "CREATE_TEMP_TRIGGER", "CREATE_TEMP_VIEW", "CREATE_TRIGGER", "CREATE_VIEW", "DELETE", "DROP_INDEX", "DROP_TABLE", "DROP_TEMP_INDEX", "DROP_TEMP_TABLE", "DROP_TEMP_TRIGGER", "DROP_TEMP_VIEW", "DROP_TRIGGER", "DROP_VIEW", "INSERT", "PRAGMA", "READ", "SELECT", "TRANSACTION", "UPDATE"];

    var_dump($constants[$action], implode(',', array_slice(func_get_args(), 1)));
    return Pdo\Sqlite::OK;
});

var_dump($db->exec('SELECT * FROM test WHERE a = 42;'));
var_dump($db->exec('DROP TABLE test;'));

// Try to return something invalid from the authorizer
$db->setAuthorizer(function () {
    return 'FAIL';
});

try {
    var_dump($db->query('SELECT 1;'));
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

$db->setAuthorizer(function () {
    return 4200;
});

try {
    var_dump($db->query('SELECT 1;'));
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(9) "SELECT 1;"
}
SQLSTATE[HY000]: General error: 23 not authorized
int(1)
int(1)
string(6) "SELECT"
string(3) ",,,"
string(4) "READ"
string(12) "test,a,main,"
string(4) "READ"
string(12) "test,a,main,"
int(1)
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
int(1)
PDO::query(): Return value of the authorizer callback must be of type int, string returned
PDO::query(): Return value of the authorizer callback must be one of Pdo\Sqlite::OK, Pdo\Sqlite::DENY, or Pdo\Sqlite::IGNORE
