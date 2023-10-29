--TEST--
MySQL PDO->__construct() - URI
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
/* TODO - fix this limitation */
if (getenv('PDO_MYSQL_TEST_DSN') !== "mysql:dbname=phptest;unix_socket=/tmp/mysql.sock")
    die("skip Fix test to run in other environments as well!");
?>
--INI--
pdo.dsn.mysql="mysql:dbname=phptest;socket=/tmp/mysql.sock"
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

    $found = false;
    $values = ini_get_all();
    foreach ($values as $name => $dsn)
        if ('pdo.dsn.mysql' == $name) {
            printf("pdo.dsn.mysql=%s\n", $dsn);
            $found = true;
            break;
        }

    if (!$found) {
        $dsn = ini_get('pdo.dsn.mysql');
        $found = ($dsn !== false);
    }

    if (!$found)
        printf("pdo.dsn.mysql cannot be accessed through ini_get_all()/ini_get()\n");

    if (MySQLPDOTest::getDSN() == $dsn) {
        // we are lucky, we can run the test
        try {

            $user = PDO_MYSQL_TEST_USER;
            $pass	= PDO_MYSQL_TEST_PASS;
            $db = new PDO('mysql', $user, $pass);

        } catch (PDOException $e) {
            printf("[001] %s, [%s] %s\n",
                $e->getMessage(),
                (is_object($db)) ? $db->errorCode() : 'n/a',
                (is_object($db)) ? implode(' ', $db->errorInfo()) : 'n/a');
        }

    }

    print "done!";
?>
--EXPECT--
pdo.dsn.mysql cannot be accessed through ini_get_all()/ini_get()
done!
