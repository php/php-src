<?php
require_once __DIR__ . '/config.inc';
require_once __DIR__ . '/../../../../ext/pdo/tests/pdo_test.inc';

foreach ($env as $k => $v) {
    define($k, $v);
}

class MySQLPDOTest extends PDOTest {

    static function factory($classname = 'PDO', $mydsn = null, $myAttr = null) {
        $dsn 	= self::getDSN($mydsn);
        $user	= PDO_MYSQL_TEST_USER;
        $pass	= PDO_MYSQL_TEST_PASS;
        $attr	= PDO_MYSQL_TEST_ATTR;

        if ($myAttr) {
            $attr = $myAttr;
        } else {
            $attr = is_string($attr) && strlen($attr) ? unserialize($attr) : null;
        }

        $db = new $classname($dsn, $user, $pass, $attr);
        if (!$db) {
            die("Could not create PDO object (DSN=$dsn, user=$user)\n");
        }

        $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
        $db->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER);

        return $db;
    }

    static function factoryWithAttr($attr) {
        return self::factory('PDO', null, $attr);
    }

    static function createTestTable($table, $db, $engine = null) {
        if (!$engine)
            $engine = PDO_MYSQL_TEST_ENGINE;

        $db->exec("DROP TABLE IF EXISTS {$table}");
        $db->exec("CREATE TABLE {$table} (id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE={$engine}");
        $db->exec("INSERT INTO {$table} (id, label) VALUES (1, 'a'), (2, 'b'), (3, 'c'), (4, 'd'), (5, 'e'), (6, 'f')");
    }

    static function getTableEngine() {
        return PDO_MYSQL_TEST_ENGINE;
    }

    static function getDSN($new_options = null, $addition = '') {
        if (!$new_options)
            return PDO_MYSQL_TEST_DSN . $addition;

        $old_options = array();
        $dsn = substr(PDO_MYSQL_TEST_DSN,
            strpos(PDO_MYSQL_TEST_DSN, ':') + 1,
            strlen(PDO_MYSQL_TEST_DSN));

        // no real parser - any exotic setting can fool us
        $parts = explode(';', $dsn);
        foreach ($parts as $k => $v) {
            $tmp = explode('=', $v);
            if (count($tmp) == 2)
                $old_options[$tmp[0]] = $tmp[1];
        }

        $options = $old_options;
        foreach ($new_options as $k => $v)
            $options[$k] = $v;

        $dsn = 'mysql:';
        foreach ($options as $k => $v)
            $dsn .= sprintf('%s=%s;', $k, $v);

        if ($addition)
            $dsn .= $addition;
        else
            $dsn = substr($dsn, 0, strlen($dsn) -1);

        return $dsn;
    }

    static function getClientVersion($db) {
        return self::extractVersion($db->getAttribute(PDO::ATTR_CLIENT_VERSION));
    }

    static function getServerVersion($db) {
        return self::extractVersion($db->getAttribute(PDO::ATTR_SERVER_VERSION));
    }

    static function extractVersion($version_string) {
        /*
        TODO:
        We're a bit in trouble: PDO_MYSQL returns version strings.
        That's wrong according to the manual. According to the manual
        integers should be returned. However, this code needs to work
        with stinky PDO_MYSQL and hopefully better PDO_MYSQLND.
        */

        // already an int value?
        if (is_int($version_string))
            return $version_string;

        // string but int value?
        $tmp = (int)$version_string;
        if (((string)$tmp) === $version_string)
            return $tmp;

        // stinky string which we need to parse
        $parts = explode('.', $version_string);
        if (count($parts) < 3)
            return -1;

        $version = (int)$parts[0] * 10000;
        $version+= (int)$parts[1] * 100;
        $version+= (int)$parts[2];

        return $version;
    }

    static function getTempDir() {
        if (!empty($_ENV['TMP']))
            return realpath( $_ENV['TMP'] );
        if (!empty($_ENV['TMPDIR']))
            return realpath( $_ENV['TMPDIR'] );
        if (!empty($_ENV['TEMP']))
            return realpath( $_ENV['TEMP'] );

        $temp_file = tempnam(md5(uniqid(rand(), TRUE)), '');
        if ($temp_file) {
            $temp_dir = realpath(dirname($temp_file));
            unlink($temp_file);
            return $temp_dir;
        }
        return false;
    }

    static function detect_transactional_mysql_engine($db) {
        foreach ($db->query("show variables like 'have%'") as $row) {
            if (!empty($row) && $row[1] == 'YES' && ($row[0] == 'have_innodb' || $row[0] == 'have_bdb')) {
                return str_replace("have_", "", $row[0]);
            }
        }
        /* MySQL 5.6.1+ */
        foreach ($db->query("SHOW ENGINES") as $row) {
            if (isset($row['engine']) && isset($row['support'])) {
                 if ('InnoDB' == $row['engine'] && ('YES' == $row['support'] || 'DEFAULT' == $row['support']))
                    return 'innodb';
            }
        }
        return false;
    }

    static function isPDOMySQLnd() {
        ob_start();
        phpinfo();
        $tmp = ob_get_contents();
        ob_end_clean();
        return (preg_match('/PDO Driver for MySQL.*enabled/', $tmp) &&
            preg_match('/Client API version.*mysqlnd/', $tmp));
    }

    static function skip() {
        try {
            $db = self::factory();
        } catch (PDOException $e) {
            die('skip could not connect');
        }
    }

    static function skipInfileNotAllowed() {
        $db = self::factory();
        $stmt = $db->query("SHOW VARIABLES LIKE 'local_infile'");
        if (($row = $stmt->fetch(PDO::FETCH_ASSOC)) && ($row['value'] != 'ON'))
            die("skip Server variable 'local_infile' seems not set to 'ON', found '". $row['value'] ."'");
    }

    static function skipVersionThanLess ($expected_version) {
        $nums = [
            (int) substr($expected_version, 0, 1),
            (int) substr($expected_version, 1, 2),
            (int) substr($expected_version, 3, 2),
        ];
        $expected_version_str = implode('.', $nums);

        $db = self::factory();
        $stmt = $db->query('SELECT VERSION() as _version');
        $row = $stmt->fetch(PDO::FETCH_ASSOC);
        $matches = array();
        if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
            die(sprintf("skip Cannot determine MySQL Server version\n"));

        $version = $matches[1] * 10000 + $matches[2] * 100 + $matches[3];
        if ($version < $expected_version)
            die(sprintf("skip Need MySQL Server %s+, found %d.%02d.%02d (%d)\n",
                $expected_version_str, $matches[1], $matches[2], $matches[3], $version));
    }

    static function skipNotMySQLnd(?string $message = null) {
        $message = $message ?? 'skip only for mysqlnd';
        if (!self::isPDOMySQLnd()) die($message);
    }

    static function skipNotTransactionalEngine(?string $message = null) {
        $db = self::factory();
        $message = $message ?? 'skip Transactional engine not found';
        if (false == self::detect_transactional_mysql_engine($db)) die($message);
    }
}
?>
