--TEST--
MySQL PDOStatement->closeCursor()
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
    /* TODO the results look wrong, why do we get 2014 with buffered AND unbuffered queries */
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();

    function pdo_mysql_stmt_closecursor($db) {

        // This one should fail. I let it fail to prove that closeCursor() makes a difference.
        // If no error messages gets printed do not know if proper usage of closeCursor() makes any
        // difference or not. That's why we need to cause an error here.
        $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);
        $stmt1 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
        // query() shall fail!
        $stmt2 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
        $stmt1->closeCursor();

        // This is proper usage of closeCursor(). It shall prevent any further error messages.
        if (MySQLPDOTest::isPDOMySQLnd()) {
            $stmt1 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
        } else {
            // see pdo_mysql_stmt_unbuffered_2050.phpt for an explanation
            unset($stmt1);
            $stmt1 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
        }
        // fetch only the first rows and let closeCursor() clean up
        $row1 = $stmt1->fetch(PDO::FETCH_ASSOC);
        $stmt1->closeCursor();

        $stmt2 = $db->prepare('UPDATE test SET label = ? WHERE id = ?');
        $stmt2->bindValue(1, "z");

        $stmt2->bindValue(2, $row1['id']);
        $stmt2->execute();
        $stmt2->closeCursor();

        $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, true);
        // check if changing the fetch mode from unbuffered to buffered will
        // cause any harm to a statement created prior to the change
        $stmt1->execute();
        $row2 = $stmt1->fetch(PDO::FETCH_ASSOC);
        $stmt1->closeCursor();
        if (!isset($row2['label']) || ('z' !== $row2['label']))
            printf("Expecting array(id => 1, label => z) got %s\n", var_export($row2, true));
        unset($stmt1);

        $stmt1 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
        // should work
        $stmt2 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
        $stmt1->closeCursor();

        $stmt1 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
        // fetch only the first rows and let closeCursor() clean up
        $row3 = $stmt1->fetch(PDO::FETCH_ASSOC);
        $stmt1->closeCursor();
        assert($row3 == $row2);

        $stmt2 = $db->prepare('UPDATE test SET label = ? WHERE id = ?');
        $stmt2->bindValue(1, "a");
        $stmt2->bindValue(2, $row1['id']);
        $stmt2->execute();
        $stmt2->closeCursor();

        $stmt1->execute();
        $row4 = $stmt1->fetch(PDO::FETCH_ASSOC);
        $stmt1->closeCursor();
        assert($row4 == $row1);

        $offset = 0;
        $stmt = $db->prepare('SELECT id, label FROM test WHERE id > ? ORDER BY id ASC LIMIT 2');
        $in = 0;
        if (!$stmt->bindParam(1, $in))
            printf("[%03d + 1] Cannot bind parameter, %s %s\n", $offset,
                $stmt->errorCode(), var_export($stmt->errorInfo(), true));

        $stmt->execute();
        $id = $label = null;

        if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
            printf("[%03d + 2] Cannot bind integer column, %s %s\n", $offset,
                $stmt->errorCode(), var_export($stmt->errorInfo(), true));

        if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
            printf("[%03d + 3] Cannot bind string column, %s %s\n", $offset,
                $stmt->errorCode(), var_export($stmt->errorInfo(), true));

        while ($stmt->fetch(PDO::FETCH_BOUND))
            printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
                $in,
                var_export($id, true), gettype($id),
                var_export($label, true), gettype($label));

        $stmt->closeCursor();
        $stmt->execute();

    }


    try {

        printf("Testing emulated PS...\n");
        $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
        if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[002] Unable to turn on emulated prepared statements\n");

        printf("Buffered...\n");
        $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, true);
        MySQLPDOTest::createTestTable($db);
        pdo_mysql_stmt_closecursor($db);

        printf("Unbuffered...\n");
        $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);
        MySQLPDOTest::createTestTable($db);
        pdo_mysql_stmt_closecursor($db);

        printf("Testing native PS...\n");
        $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
        if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[002] Unable to turn off emulated prepared statements\n");

        printf("Buffered...\n");
        MySQLPDOTest::createTestTable($db);
        $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, true);
        pdo_mysql_stmt_closecursor($db);

        printf("Unbuffered...\n");
        MySQLPDOTest::createTestTable($db);
        $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);
        pdo_mysql_stmt_closecursor($db);

    } catch (PDOException $e) {
        printf("[001] %s [%s] %s\n",
            $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
Testing emulated PS...
Buffered...

Warning: PDO::query(): SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute. in %s on line %d
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Unbuffered...

Warning: PDO::query(): SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute. in %s on line %d
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Testing native PS...
Buffered...

Warning: PDO::query(): SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute. in %s on line %d
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
Unbuffered...

Warning: PDO::query(): SQLSTATE[HY000]: General error: 2014 Cannot execute queries while other unbuffered queries are active.  Consider using PDOStatement::fetchAll().  Alternatively, if your code is only ever going to run against mysql, you may enable query buffering by setting the PDO::MYSQL_ATTR_USE_BUFFERED_QUERY attribute. in %s on line %d
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
done!
