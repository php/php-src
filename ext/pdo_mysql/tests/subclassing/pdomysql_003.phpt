--TEST--
PDO_mysql getWarningCount
--EXTENSIONS--
pdo_mysql
--FILE--
<?php

require_once __DIR__ . "/../config_functions.inc";

if (class_exists(PdoMysql::class) === false) {
    echo "PdoMysql class does not exist.\n";
    exit(-1);
}

[$dsn, $user, $pass] = getDsnUserAndPassword();

$db =  Pdo::connect($dsn, $user, $pass);

if (!$db instanceof PdoMysql) {
    echo "Wrong class type. Should be PdoMysql but is [" . get_class($db) . "\n";
}

$assertWarnings = function ($db, $q, $count) {
    $db->query($q);
    printf("Query %s produced %d warnings\n", $q, $db->getWarningCount());
};
$assertWarnings($db, 'SELECT 1 = 1', 0);
$assertWarnings($db, 'SELECT 1 = "A"', 1);

--EXPECT--
Query SELECT 1 = 1 produced 0 warnings
Query SELECT 1 = "A" produced 1 warnings
