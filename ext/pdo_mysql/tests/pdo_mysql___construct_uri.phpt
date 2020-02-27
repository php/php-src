--TEST--
MySQL PDO->__construct() - URI
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

    try {

        if ($tmp = MySQLPDOTest::getTempDir()) {

            $file = $tmp . DIRECTORY_SEPARATOR . 'pdomuri.tst';
            $dsn = MySQLPDOTest::getDSN();
            $user = PDO_MYSQL_TEST_USER;
            $pass = PDO_MYSQL_TEST_PASS;
            $uri = sprintf('uri:file://%s', (substr(PHP_OS, 0, 3) == 'WIN' ? str_replace('\\', '/', $file) : $file));

            if ($fp = @fopen($file, 'w')) {
                // ok, great we can create a file with a DSN in it
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
                fwrite($fp, sprintf('mysql:dbname=letshopeinvalid;%s%s',
                    chr(0), $dsn));
                fclose($fp);
                clearstatcache();
                assert(file_exists($file));
                try {
                    $db = new PDO($uri, $user, $pass);
                } catch (PDOException $e) {
                    $expected = array(
                        "SQLSTATE[HY000] [1049] Unknown database 'letshopeinvalid'",
                        "SQLSTATE[HY000] [2002] No such file or directory"
                    );
                    printf("[003] URI=%s, DSN=%s, File=%s (%d bytes, '%s'), chr(0) test, %s\n",
                    $uri, $dsn,
                    $file, filesize($file), file_get_contents($file),
                    (in_array($e->getMessage(), $expected) ? 'EXPECTED ERROR' : $e->getMessage()));
                }
                unlink($file);
            }

        }

        /* TODO: safe mode */

    } catch (PDOException $e) {
        printf("[001] %s, [%s] %s\n",
            $e->getMessage(),
            (is_object($db)) ? $db->errorCode() : 'n/a',
            (is_object($db)) ? implode(' ', $db->errorInfo()) : 'n/a');
    }

    print "done!";
?>
--EXPECTF--
[003] URI=uri:file://%spdomuri.tst, DSN=mysql%sdbname=%s, File=%spdomuri.tst (%d bytes, 'mysql%sdbname=letshopeinvalid%s'), chr(0) test, EXPECTED ERROR
done!
