--TEST--
Bug #39858 (Lost connection to MySQL server during query by a repeated call stored proced)
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

function bug_39858($db) {
    $procedure = 'bug_39858_pdo_mysql_p';

    $db->exec("DROP PROCEDURE IF EXISTS {$procedure}");
    $db->exec("
        CREATE PROCEDURE {$procedure}()
            NOT DETERMINISTIC
            CONTAINS SQL
            SQL SECURITY DEFINER
            COMMENT ''
        BEGIN
            SELECT 2 * 2;
        END;");

    $stmt = $db->prepare("CALL {$procedure}()");
    $stmt->execute();
    do {
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
    } while ($stmt->nextRowset());

    $stmt = $db->prepare("CALL {$procedure}()");
    $stmt->execute();
    do {
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
    } while ($stmt->nextRowset());
    $stmt->closeCursor();
}

printf("Emulated Prepared Statements...\n");
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
bug_39858($db);

printf("Native Prepared Statements...\n");
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
bug_39858($db);

print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec("DROP PROCEDURE IF EXISTS bug_39858_pdo_mysql_p");
?>
--EXPECT--
Emulated Prepared Statements...
array(1) {
  [0]=>
  array(1) {
    ["2 * 2"]=>
    string(1) "4"
  }
}
array(0) {
}
array(1) {
  [0]=>
  array(1) {
    ["2 * 2"]=>
    string(1) "4"
  }
}
array(0) {
}
Native Prepared Statements...
array(1) {
  [0]=>
  array(1) {
    ["2 * 2"]=>
    string(1) "4"
  }
}
array(0) {
}
array(1) {
  [0]=>
  array(1) {
    ["2 * 2"]=>
    string(1) "4"
  }
}
array(0) {
}
done!
