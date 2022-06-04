--TEST--
PdoPgsql getWarningCount
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require_once __DIR__ . "/config.inc";

$db =  Pdo::connect($config['ENV']['PDOTEST_DSN']);
if (!$db instanceof PdoPgsql) {
    echo "Wrong class type. Should be PdoPgsql but is " . get_class($db) . "\n";
}

$result = $db->escapeIdentifier("This is a quote\"");

// TODO - ask someone who knows about postgresql if this is correct:
echo "Result is [$result]\n";
--EXPECT--
Result is ["This is a quote"""]
