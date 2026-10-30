--TEST--
MySQL PDO->errorInfo()
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
    $db = MySQLPDOTest::factory();

    $table = 'pdo_mysql_errorinfo';
    MySQLPDOTest::createTestTable($table, $db);

    function check_error($offset, &$obj, $expected = '00000') {
        $info = $obj->errorInfo();
        $code = $info[0];

        if (($code != $expected) && (($expected != '00000') && ($code != ''))) {
            printf("[%03d] Expecting error code '%s' got code '%s'\n",
                $offset, $expected, $code);
        }

        if ($expected != '00000') {
            if (!isset($info[1]) || $info[1] == '')
                printf("[%03d] Driver-specific error code not set\n", $offset);
            if (!isset($info[2]) || $info[2] == '')
                printf("[%03d] Driver-specific error message.not set\n", $offset);
        }
    }

    function pdo_mysql_errorinfo($db, $offset) {
        global $table;

        try {
            /*
            If you create a PDOStatement object through PDO->prepare()
            or PDO->query() and invoke an error on the statement handle,
            PDO->errorCode() will not reflect that error. You must call
            PDOStatement->errorCode() to return the error code for an
            operation performed on a particular statement handle.
            */
            $code = $db->errorCode();
            check_error($offset + 2, $db);

            $stmt = $db->query("SELECT id, label FROM {$table}");
            $stmt2 = &$stmt;
            check_error($offset + 3, $db);
            check_error($offset + 4, $stmt);

            $db->exec("DROP TABLE IF EXISTS {$table}");
            @$stmt->execute();
            check_error($offset + 5, $db);
            check_error($offset + 6, $stmt, '42S02');
            check_error($offset + 7, $stmt2, '42S02');

            @$stmt = $db->query('SELECT id, label FROM unknown');
            check_error($offset + 8, $db, '42S02');

            MySQLPDOTest::createTestTable($table, $db);
            $stmt = $db->query("SELECT id, label FROM {$table}");
            check_error($offset + 9, $db);
            check_error($offset + 10, $stmt);

            $db2 = &$db;
            $db->exec('DROP TABLE IF EXISTS pdo_mysql_errorinfo_unknown');
            @$db->query('SELECT id, label FROM pdo_mysql_errorinfo_unknown');
            check_error($offset + 11, $db, '42S02');
            check_error($offset + 12, $db2, '42S02');
            check_error($offset + 13, $stmt);
            check_error($offset + 14, $stmt2);

            // lets hope this is an invalid attribute code
            $invalid_attr = -1 * PHP_INT_MAX + 3;
            $tmp = @$db->getAttribute($invalid_attr);
            check_error($offset + 15, $db, 'IM001');
            check_error($offset + 16, $db2, 'IM001');
            check_error($offset + 17, $stmt);
            check_error($offset + 18, $stmt2);

        } catch (PDOException $e) {
            printf("[%03d] %s [%s] %s\n",
                $offset + 19, $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
        }
    }

    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
    printf("Emulated Prepared Statements...\n");
    pdo_mysql_errorinfo($db, 0);

    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
    printf("Native Prepared Statements...\n");
    pdo_mysql_errorinfo($db, 20);

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS pdo_mysql_errorinfo');
?>
--EXPECT--
Emulated Prepared Statements...
[015] Driver-specific error code not set
[015] Driver-specific error message.not set
[016] Driver-specific error code not set
[016] Driver-specific error message.not set
Native Prepared Statements...
done!
