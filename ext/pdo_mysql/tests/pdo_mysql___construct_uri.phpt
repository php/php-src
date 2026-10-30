--TEST--
MySQL PDO->__construct() - URI
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once __DIR__ . '/inc/mysql_pdo_test.inc';

    try {

        if ($tmp = MySQLPDOTest::getTempDir()) {
            $file = $tmp . DIRECTORY_SEPARATOR . 'pdomuri.tst';
            $dsn = MySQLPDOTest::getDSN();
            $user = PDO_MYSQL_TEST_USER;
            $pass = PDO_MYSQL_TEST_PASS;
            $uri = 'uri:file://' . $file;

            if ($fp = @fopen($file, 'w')) {
                fwrite($fp, $dsn);
                fclose($fp);
                clearstatcache();
                assert(file_exists($file));
                try {
                    $db = new PDO($uri, $user, $pass);
                } catch (PDOException $e) {
                    printf("[002] URI=%s, DSN=%s, File=%s (%d bytes, '%s'), %s\n",
                        $uri, $dsn,
                        $file, filesize($file), file_get_contents($file),
                        $e->getMessage());
                }
                unlink($file);
            }

            if ($fp = @fopen($file, 'w')) {
                fwrite($fp, $dsn . chr(0) . ';host=nonsense;unix_socket=nonsense');
                fclose($fp);
                clearstatcache();
                assert(file_exists($file));
                try {
                    $db = new PDO($uri, $user, $pass);
                } catch (PDOException $e) {
                    printf("[003] URI=%s, DSN=%s, File=%s (%d bytes, '%s'), %s\n",
                        $uri, $dsn,
                        $file, filesize($file), file_get_contents($file),
                        $e->getMessage());
                }
                unlink($file);
            }

        }

    } catch (PDOException $e) {
        printf("[001] %s, [%s] %s\n",
            $e->getMessage(),
            (is_object($db)) ? $db->errorCode() : 'n/a',
            (is_object($db)) ? implode(' ', $db->errorInfo()) : 'n/a');
    }

    print "done!";
?>
--EXPECT--
done!
