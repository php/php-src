<?php
require_once __DIR__ . '/config.inc';
require_once __DIR__ . '/../../../../ext/pdo/tests/pdo_test.inc';

foreach ($env as $k => $v) {
    define($k, $v);
}

class ODBCPDOTest extends PDOTest {

    static function factory($classname = 'PDO') {
        $dsn  = PDO_ODBC_TEST_DSN;
        $user = PDO_ODBC_TEST_USER;
        $pass = PDO_ODBC_TEST_PASS;
        $attr = PDO_ODBC_TEST_ATTR;

        $attr = is_string($attr) && strlen($attr) ? unserialize($attr) : null;

        $db = new $classname($dsn, $user, $pass, $attr);
        if (!$db) {
            die("Could not create PDO object (DSN=$dsn, user=$user)\n");
        }

        $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
        $db->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER);
        $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

        return $db;
    }

    static function skip() {
        if (substr(PHP_OS, 0, 3) == 'WIN' &&
            PDO_ODBC_TEST_DSN === '' &&
            !extension_loaded('com_dotnet')
        ) {
            die('skip - either PDOTEST_DSN or com_dotnet extension is needed to setup the connection');
        }

        try {
            $db = self::factory();
        } catch (PDOException $e) {
            die('skip could not connect');
        }
    }

    static function skipWithUnixODBC() {
        if (PDO_ODBC_TYPE === "unixODBC") {
            die("skip Fails with unixODBC");
        }
    }

    static function skipNoDirect() {
        $dsn = PDO_ODBC_TEST_DSN;
        if (!$dsn || strpos($dsn, '=') === false) {
            die('skip this test is for direct connections only.');
        }
    }

    static function skipToofewCredentials() {
        if (PDO_ODBC_TEST_DSN === '' || PDO_ODBC_TEST_USER === null || PDO_ODBC_TEST_PASS === null) {
            die('skip too few credentials.');
        }
    }
}
?>
