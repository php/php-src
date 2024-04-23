--TEST--
PgSQL PDO Parser custom syntax
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require_once __DIR__ . "/config.inc";

$db = new PdoPgsql($config['ENV']['PDOTEST_DSN']);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$query = <<<EOF
SELECT e'He''ll\'o ' || ? AS b -- '
UNION ALL
SELECT 'He''ll''o\' || ? AS b -- '
EOF;

$stmt = $db->prepare($query);
$stmt->execute(['World', 'World']);

while ($row = $stmt->fetchColumn()) {
    var_dump($row);
}

?>
--EXPECT--
string(13) "He'll'o World"
string(13) "He'll'o\World"
