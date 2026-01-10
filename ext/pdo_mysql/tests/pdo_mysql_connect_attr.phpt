--TEST--
PDO_MYSQL: check the session_connect_attrs table for connection attributes
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipNotMySQLnd();

$pdo = MySQLPDOTest::factory();

$stmt = $pdo->query("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema='performance_schema' AND table_name='session_connect_attrs'");
if (!$stmt || !$stmt->fetchColumn()) {
    die("skip mysql does not support session_connect_attrs table yet");
}

$stmt = $pdo->query("SHOW VARIABLES LIKE 'performance_schema'");
if (!$stmt || $stmt->fetchColumn(1) !== 'ON') {
    die("skip performance_schema is OFF");
}
?>
--FILE--
<?php

require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();

if (preg_match('/host=([^;]+)/', PDO_MYSQL_TEST_DSN, $m)) {
    $host = $m[1];
}

//in case $host is empty, do not test for _server_host field
if (isset($host) && $host !== '') {
    $stmt = $pdo->query("SELECT * FROM performance_schema.session_connect_attrs WHERE ATTR_NAME='_server_host' AND processlist_id = connection_id()");

    $row = $stmt->fetch(PDO::FETCH_ASSOC);

    if (!$row || !isset($row['attr_name'])) {
        echo "_server_host missing\n";
    } elseif ($row['attr_value'] !== $host) {
        printf("_server_host mismatch (expected '%s', got '%s')\n", $host, $row['attr_value']);
    }
}

$stmt = $pdo->query("SELECT * FROM performance_schema.session_connect_attrs WHERE ATTR_NAME='_client_name' AND processlist_id = connection_id()");

$row = $stmt->fetch(PDO::FETCH_ASSOC);
if (!$row || !isset($row['attr_name'])) {
    echo "_client_name missing\n";
} elseif ($row['attr_value'] !== 'mysqlnd') {
    printf("_client_name mismatch (expected 'mysqlnd', got '%s')\n", $row['attr_value']);
}

printf("done!");
?>
--EXPECT--
done!
