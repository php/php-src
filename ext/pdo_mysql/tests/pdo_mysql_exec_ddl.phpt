--TEST--
MySQL PDO->exec(), affected rows
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
    function exec_and_count($offset, &$db, $sql, $exp, $suppress_warning = false) {
        try {
            if ($suppress_warning)
                $ret = @$db->exec($sql);
            else
                $ret = $db->exec($sql);

            if ($ret !== $exp) {
                printf("[%03d] Expecting '%s'/%s got '%s'/%s when running '%s', [%s] %s\n",
                    $offset, $exp, gettype($exp), $ret, gettype($ret), $sql,
                    $db->errorCode(), implode(' ', $db->errorInfo()));
                return false;
            }

        } catch (PDOException $e) {
            printf("[%03d] '%s' has failed, [%s] %s\n",
                $offset, $sql, $db->errorCode(), implode(' ', $db->errorInfo()));
            return false;
        }

        return true;
    }

    require_once __DIR__ . '/inc/mysql_pdo_test.inc';
    $db = MySQLPDOTest::factory();

    $db_name = 'pdo_mysql_exec_ddl_db';
    $db_name_2 = 'pdo_mysql_exec_ddl_db_2';
    $table = 'pdo_mysql_exec_ddl';
    $table2 = 'pdo_mysql_exec_ddl_2';
    /* affected rows related */
    try {
        @$db->exec("DROP DATABASE IF EXISTS {$db_name}");
        @$db->exec("DROP DATABASE IF EXISTS {$db_name_2}");
        if (1 === @$db->exec("CREATE DATABASE {$db_name}")) {
            // yippie - we can create databases etc.
            exec_and_count(3, $db, "ALTER DATABASE {$db_name} CHARACTER SET latin1", 1);
        }

        exec_and_count(4, $db, "DROP TABLE IF EXISTS {$table}", 0);
        exec_and_count(5, $db, "DROP TABLE IF EXISTS {$table2}", 0);
        if (0 === $db->exec("CREATE TABLE {$table} (id INT, col1 CHAR(2))")) {
            exec_and_count(5, $db, "CREATE INDEX idx1 ON {$table} (id)", 0);
            exec_and_count(6, $db, "DROP INDEX idx1 ON {$table}", 0);
            exec_and_count(7, $db, "ALTER TABLE {$table} DROP id", 0);
            exec_and_count(8, $db, "ALTER TABLE {$table} ADD id INT", 0);
            exec_and_count(9, $db, "ALTER TABLE {$table} ALTER id SET DEFAULT 1", 0);
            exec_and_count(10, $db, "RENAME TABLE {$table} TO {$table2}", 0);
        }

        /*
        11.1.2. ALTER LOGFILE GROUP Syntax
        11.1.3. ALTER SERVER Syntax
        11.1.5. ALTER TABLESPACE Syntax
        11.1.8. CREATE LOGFILE GROUP Syntax
        11.1.9. CREATE SERVER Syntax
        11.1.11. CREATE TABLESPACE Syntax
        11.1.14. DROP LOGFILE GROUP Syntax
        11.1.15. DROP SERVER Syntax
        11.1.17. DROP TABLESPACE Syntax
        */

    } catch (PDOException $e) {
        printf("[001] %s, [%s] %s\n",
            $e->getMessage(),
            $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_exec_ddl');
$db->exec('DROP TABLE IF EXISTS pdo_mysql_exec_ddl_2');
$db->exec('DROP DATABASE IF EXISTS pdo_mysql_exec_ddl_db');
$db->exec('DROP DATABASE IF EXISTS pdo_mysql_exec_ddl_db_2');
?>
--EXPECT--
done!
