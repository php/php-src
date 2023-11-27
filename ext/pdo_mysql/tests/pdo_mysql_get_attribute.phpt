--TEST--
MySQL PDO->getAttribute()
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipNotTransactionalEngine();
--FILE--
<?php
    require_once __DIR__ . '/inc/mysql_pdo_test.inc';
    $db = MySQLPDOTest::factory();

    function set_and_get($offset, $db, $attribute, $value) {
        $value_type = gettype($value);
        try {
            if (!$db->setAttribute($attribute, $value)) {
                printf("[%03d] Cannot set attribute '%s' to value '%s'\n",
                    $offset, $attribute, var_export($tmp, true));
                return false;
            }

            if (gettype($value) != $value_type) {
                printf("[%03d] Call to PDO::setAttribute(int attribute, mixed value) has changed the type of value from %s to %s, test will not work properly\n",
                    $offset, $value_type, gettype($value));
                return false;
            }

            $tmp = $db->getAttribute($attribute);
            if ($tmp !== $value) {
                printf("[%03d] Attribute '%s' was set to '%s'/%s but getAttribute() reports '%s'/%s\n",
                    $offset, $attribute, var_export($value, true), gettype($value), var_export($tmp, true), gettype($tmp));
                return false;
            }
        } catch (PDOException $e) {
            printf("[%03d] %s, [%s] %s\n",
                $offset, $e->getMessage(),
                $db->errorCode(), implode(' ', $db->errorInfo()));
            return false;
        }

        return true;
    }

    set_and_get(1, $db, PDO::ATTR_AUTOCOMMIT, 1);
    /*
    set_and_get(2, $db, PDO::ATTR_AUTOCOMMIT, 0);
    set_and_get(3, $db, PDO::ATTR_AUTOCOMMIT, -1);
    $obj = new stdClass();
    set_and_get(4, $db, PDO::ATTR_AUTOCOMMIT, $obj);

    set_and_get(5, $db, PDO::MYSQL_ATTR_LOCAL_INFILE, 1);
    set_and_get(6, $db, PDO::MYSQL_ATTR_LOCAL_INFILE, 0);
    set_and_get(7, $db, PDO::MYSQL_ATTR_LOCAL_INFILE, -1);
    $tmp = array();
    set_and_get(8, $db, PDO::MYSQL_ATTR_LOCAL_INFILE, $tmp);

    set_and_get(9, $db, PPDO::MYSQL_ATTR_INIT_COMMAND, '');
    set_and_get(10, $db, PPDO::MYSQL_ATTR_INIT_COMMAND, 'SOME SQL');
    set_and_get(11, $db, PPDO::MYSQL_ATTR_INIT_COMMAND, -1);
    */
    /*
    PDO::MYSQL_ATTR_READ_DEFAULT_FILE (integer)

    Read options from the named option file instead of from my.cnf.
    PDO::MYSQL_ATTR_READ_DEFAULT_GROUP (integer)

    Read options from the named group from my.cnf or the file specified with MYSQL_READ_DEFAULT_FILE.
    PDO::MYSQL_ATTR_MAX_BUFFER_SIZE (integer)

    Maximum buffer size. Defaults to 1 MiB.
    PDO::MYSQL_ATTR_DIRECT_QUERY (integer)

    Perform direct queries, don't use prepared statements.
    */
    /*
    TODO - read only
    PDO::ATTR_CONNECTION_STATUS
    PDO::ATTR_SERVER_INFO
    */

    print "done!";
?>
--EXPECT--
done!
