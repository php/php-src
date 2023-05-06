--TEST--
PdoPgSql getWarningCount
--EXTENSIONS--
pdo_mysql
--FILE--
<?php

require_once __DIR__ . "/../config_functions.inc";

if (class_exists(PdoPgSql::class) === false) {
    echo "PdoPgSql class does not exist.\n";
    exit(-1);
}

$dsn = getDsn();

$db =  Pdo::connect($dsn);

if (!$db instanceof PdoPgSql) {
    echo "Wrong class type. Should be PdoPgSql but is [" . get_class($db) . "\n";
}

$result = $db->escapeIdentifier("This is a quote\"");

// TODO - ask someone who knows about postgresql if this is correct:
echo "Result is [$result]\n";

--EXPECT--
Result is ["This is a quote"""]

