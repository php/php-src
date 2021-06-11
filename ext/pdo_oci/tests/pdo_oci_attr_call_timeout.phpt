--TEST--
PDO_OCI: Attribute: Setting and using call timeout
--EXTENSIONS--
pdo
pdo_oci
--SKIPIF--
<?php
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
if (strcasecmp(getenv('PDOTEST_USER'), "system") && strcasecmp(getenv('PDOTEST_USER'), "sys")) {
    die("skip needs to be run as a user with access to DBMS_LOCK");
}

$dbh = PDOTest::factory();
preg_match('/^[[:digit:]]+/', $dbh->getAttribute(PDO::ATTR_CLIENT_VERSION), $matches);
if (!(isset($matches[0]) && $matches[0] >= 18)) {
    die("skip works only with Oracle 18c or greater version of Oracle client libraries");
}

?>
--FILE--
<?php

require(__DIR__ . '/../../pdo/tests/pdo_test.inc');

function mysleep($dbh, $t)
{
    $stmt = $dbh->prepare("begin dbms_lock.sleep(:t); end;");

    if (!$stmt) {
        $error = $dbh->errorInfo();
        echo "Prepare error was ", $error[2], "\n";
        return;
    }
    $stmt->bindParam(":t", $t, PDO::PARAM_INT);

    $r = $stmt->execute();
    if ($r) {
        echo "Execute succeeded\n";
    } else {
        $error = $dbh->errorInfo();
        echo "Execute error was ", $error[2], "\n";
    }
}

$dbh = PDOTest::factory();
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

echo "Test 1\n";

$dbh->setAttribute(PDO::OCI_ATTR_CALL_TIMEOUT, 4000); // milliseconds

echo "call timeout:\n";
var_dump($dbh->getAttribute(PDO::OCI_ATTR_CALL_TIMEOUT));

$r = mysleep($dbh, 8); // seconds

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
call timeout:
int(4000)
Execute error was OCIStmtExecute: ORA-%r(03136|03156)%r: %s
 (%s:%d)
===DONE===
