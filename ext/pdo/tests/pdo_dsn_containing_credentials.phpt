--TEST--
PDO Common: Pass credentials in dsn instead of constructor params
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');

$driver = substr(getenv('PDOTEST_DSN'), 0, strpos(getenv('PDOTEST_DSN'), ':'));
if (!in_array($driver, array('mssql','sybase','dblib','firebird','mysql','oci')))
    die('skip not supported');

require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
    require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

    $orgDsn = getenv('PDOTEST_DSN');
    $orgUser = getenv('PDOTEST_USER');
    $orgPass = getenv('PDOTEST_PASS');

    try
    {
        putenv("PDOTEST_DSN=$orgDsn;user=$orgUser;password=$orgPass");
        putenv("PDOTEST_USER");
        putenv("PDOTEST_PASS");

        $link = PDOTest::factory();
        echo "using credentials in dsn: done\n";


        // test b/c - credentials in DSN are ignored when user/pass passed as separate params
        putenv("PDOTEST_DSN=$orgDsn;user=incorrect;password=ignored");
        putenv("PDOTEST_USER=$orgUser");
        putenv("PDOTEST_PASS=$orgPass");

        $link = PDOTest::factory();
        echo "ignoring credentials in dsn: done\n";
    }
    finally
    {
        putenv("PDOTEST_DSN=$orgDsn");
        putenv("PDOTEST_USER=$orgUser");
        putenv("PDOTEST_PASS=$orgPass");
    }
?>
--EXPECT--
using credentials in dsn: done
ignoring credentials in dsn: done
