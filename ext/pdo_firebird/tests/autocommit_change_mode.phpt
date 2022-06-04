--TEST--
PDO_Firebird: change auto commit
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require("testdb.inc");
$dbh = getDbConnection();
$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, true);

echo "========== not in manually transaction ==========\n";

echo "auto commit ON from ON\n";
$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, true);
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));
echo "Success\n\n";

echo "auto commit OFF from ON\n";
$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, false);
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));
echo "Success\n\n";

echo "auto commit OFF from OFF\n";
$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, false);
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));
echo "Success\n\n";

echo "auto commit ON from OFF\n";
$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, true);
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));
echo "Success\n\n";

echo "========== in manually transaction ==========\n";

echo "begin transaction\n";
$dbh->beginTransaction();
echo "\n";

echo "auto commit ON from ON, expect error\n";
try {
    $dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, true);
} catch (PDOException $e) {
    var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));
    echo $e->getMessage()."\n\n";
}

echo "auto commit OFF from ON, expect error\n";
try {
    $dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, false);
} catch (PDOException $e) {
    var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));
    echo $e->getMessage()."\n\n";
}

echo "end transaction\n";
$dbh->rollback();

echo "auto commit OFF\n";
$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, false);

echo "begin transaction\n";
$dbh->beginTransaction();
echo "\n";

echo "auto commit ON from OFF, expect error\n";
try {
    $dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, true);
} catch (PDOException $e) {
    var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));
    echo $e->getMessage()."\n\n";
}

echo "auto commit OFF from OFF, expect error\n";
try {
    $dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, false);
} catch (PDOException $e) {
    var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));
    echo $e->getMessage()."\n\n";
}

echo "end transaction\n";
$dbh->rollback();
echo "\n";

echo "done!";
?>
--EXPECT--
========== not in manually transaction ==========
auto commit ON from ON
bool(true)
Success

auto commit OFF from ON
bool(false)
Success

auto commit OFF from OFF
bool(false)
Success

auto commit ON from OFF
bool(true)
Success

========== in manually transaction ==========
begin transaction

auto commit ON from ON, expect error
bool(true)
SQLSTATE[HY000]: General error: Cannot change autocommit mode while a transaction is already open

auto commit OFF from ON, expect error
bool(true)
SQLSTATE[HY000]: General error: Cannot change autocommit mode while a transaction is already open

end transaction
auto commit OFF
begin transaction

auto commit ON from OFF, expect error
bool(false)
SQLSTATE[HY000]: General error: Cannot change autocommit mode while a transaction is already open

auto commit OFF from OFF, expect error
bool(false)
SQLSTATE[HY000]: General error: Cannot change autocommit mode while a transaction is already open

end transaction

done!
