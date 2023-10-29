<?php
$env = [
    'PDO_MYSQL_TEST_DSN' => false !== getenv('PDO_MYSQL_TEST_DSN') ? getenv('PDO_MYSQL_TEST_DSN') : 'mysql:host=localhost;dbname=test',
    'PDO_MYSQL_TEST_USER' => false !== getenv('PDO_MYSQL_TEST_USER') ? getenv('PDO_MYSQL_TEST_USER') : 'root',
    'PDO_MYSQL_TEST_PASS' => false !== getenv('PDO_MYSQL_TEST_PASS') ? getenv('PDO_MYSQL_TEST_PASS') : '',
    'PDO_MYSQL_TEST_ENGINE' => false !== getenv('PDO_MYSQL_TEST_ENGINE') ? getenv('PDO_MYSQL_TEST_ENGINE') : 'InnoDB',
    'PDO_MYSQL_TEST_HOST' => false !== getenv('PDO_MYSQL_TEST_HOST') ? getenv('PDO_MYSQL_TEST_HOST') : 'localhost',
    'PDO_MYSQL_TEST_PORT' => false !== getenv('PDO_MYSQL_TEST_PORT') ? getenv('PDO_MYSQL_TEST_PORT') : null,
    'PDO_MYSQL_TEST_DB' => false !== getenv('PDO_MYSQL_TEST_DB') ? getenv('PDO_MYSQL_TEST_DB') : 'test',
    'PDO_MYSQL_TEST_SOCKET' => false !== getenv('PDO_MYSQL_TEST_SOCKET') ? getenv('PDO_MYSQL_TEST_SOCKET') : null,
    'PDO_MYSQL_TEST_CHARSET' => false !== getenv('PDO_MYSQL_TEST_CHARSET') ? getenv('PDO_MYSQL_TEST_CHARSET') : null,
];

if (false !== getenv('PDO_MYSQL_TEST_ATTR')) {
    $env['PDO_MYSQL_TEST_ATTR'] = getenv('PDO_MYSQL_TEST_ATTR');
}
?>
