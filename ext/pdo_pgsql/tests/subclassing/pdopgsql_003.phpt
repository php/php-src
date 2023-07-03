--TEST--
PdoPgsql getWarningCount
--EXTENSIONS--
pdo_mysql
--FILE--
<?php

require_once __DIR__ . "/../config_functions.inc";

if (class_exists(PdoPgsql::class) === false) {
    echo "PdoPgsql class does not exist.\n";
    exit(-1);
}

$dsn = getDsn();

$db =  Pdo::connect($dsn);

if (!$db instanceof PdoPgsql) {
    echo "Wrong class type. Should be PdoPgsql but is [" . get_class($db) . "\n";
}

$result = $db->escapeIdentifier("This is a quote\"");

// TODO - ask someone who knows about postgresql if this is correct:
echo "Result is [$result]\n";

--EXPECT--
Result is ["This is a quote"""]

