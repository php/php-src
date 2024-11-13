--TEST--
MySQL PDO->prepare(),native PS, anonymous placeholder
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
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

    try {
        $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
        if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[002] Unable to switch on emulated prepared statements, test will fail\n");

        $db->exec(sprintf('CREATE TABLE test_prepare_native_named_placeholder_everywhere(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
        $db->exec("INSERT INTO test_prepare_native_named_placeholder_everywhere(id, label) VALUES (1, 'row1')");

        $stmt = $db->prepare('SELECT ?, id, label FROM test_prepare_native_named_placeholder_everywhere WHERE ? = ? ORDER BY id ASC');
        $stmt->execute(array('id', 'label', 'label'));
        if ('00000' !== $stmt->errorCode())
            printf("[003] Execute has failed, %s %s\n",
                var_export($stmt->errorCode(), true),
                var_export($stmt->errorInfo(), true));
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        // now the same with native PS
        printf("now the same with native PS\n");
        $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
        if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[004] Unable to switch off emulated prepared statements, test_prepare_native_named_placeholder_everywhere will fail\n");

        $stmt = $db->prepare('SELECT ?, id, label FROM test_prepare_native_named_placeholder_everywhere WHERE ? = ? ORDER BY id ASC');
        $stmt->execute(array('id', 'label', 'label'));
        if ('00000' !== $stmt->errorCode())
            printf("[005] Execute has failed, %s %s\n",
                var_export($stmt->errorCode(), true),
                var_export($stmt->errorInfo(), true));

        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    } catch (PDOException $e) {
        printf("[001] %s [%s] %s\n",
            $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_prepare_native_named_placeholder_everywhere');
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(4) "row1"
  }
}
now the same with native PS
array(1) {
  [0]=>
  array(3) {
    ["?"]=>
    string(2) "id"
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(4) "row1"
  }
}
done!
