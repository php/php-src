--TEST--
Handling OCI_SUCCESS_WITH_INFO
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
?>
--FILE--
<?php

function connectAsAdmin(): PDO {
    return PDOTest::test_factory(__DIR__ . '/../../pdo_oci/tests/common.phpt');
}

function connectAsUser(string $username, string $password): PDO {
    return new PDO(getenv('PDOTEST_DSN'), $username, $password);
}

function dropProfile(PDO $conn): void {
    $conn->exec(<<<'SQL'
BEGIN
    EXECUTE IMMEDIATE 'DROP PROFILE BUG77120_PROFILE CASCADE';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -2380 THEN
            RAISE;
        END IF;
END;
SQL
    );
}

function dropUser(PDO $conn): void {
    $conn->exec(<<<'SQL'
BEGIN
    EXECUTE IMMEDIATE 'DROP USER BUG77120_USER CASCADE';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -1918 THEN
            RAISE;
        END IF;
END;
SQL
    );
}

require __DIR__ . '/../../pdo/tests/pdo_test.inc';

$conn = connectAsAdmin();

dropUser($conn);
dropProfile($conn);

$password = bin2hex(random_bytes(8));

$conn->exec('CREATE PROFILE BUG77120_PROFILE LIMIT PASSWORD_LIFE_TIME 1/86400 PASSWORD_GRACE_TIME 1');
$conn->exec('CREATE USER BUG77120_USER IDENTIFIED BY "' . $password . '" PROFILE BUG77120_PROFILE');
$conn->exec('GRANT CREATE SESSION TO BUG77120_USER');

// let the password expire
sleep(2);

$conn = connectAsUser('BUG77120_USER', $password);
var_dump($conn->errorInfo());

$conn = connectAsAdmin();
dropUser($conn);
dropProfile($conn);

?>
--EXPECTF--
array(3) {
  [0]=>
  string(5) "HY000"
  [1]=>
  int(28002)
  [2]=>
  string(%d) "OCISessionBegin: OCI_SUCCESS_WITH_INFO: ORA-28002: %s
 (%s:%d)"
}
