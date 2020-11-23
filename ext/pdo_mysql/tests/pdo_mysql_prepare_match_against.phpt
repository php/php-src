--TEST--
Bug #41876 (bindParam() and bindValue() do not work with MySQL MATCH () AGAINST ())
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();

    try {

        $db->exec('DROP TABLE IF EXISTS test');
        $db->exec('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=MyISAM');
        $db->exec('CREATE FULLTEXT INDEX idx1 ON test(label)');

        $stmt = $db->prepare('SELECT id, label FROM test WHERE MATCH label AGAINST (:placeholder)');
        $stmt->execute(array(':placeholder' => 'row'));
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        $stmt = $db->prepare('SELECT id, label FROM test WHERE MATCH label AGAINST (:placeholder)');
        $stmt->execute(array('placeholder' => 'row'));
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        $stmt = $db->prepare('SELECT id, label FROM test WHERE MATCH label AGAINST (?)');
        $stmt->execute(array('row'));
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    } catch (PDOException $e) {

        printf("[001] %s, [%s} %s\n",
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
--EXPECT--
array(0) {
}
array(0) {
}
array(0) {
}
done!
