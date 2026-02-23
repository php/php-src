<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

# Note: code below is written to be compatible in PHP 5.1+ (and sqlite of that era) (when PDO was introduced)
# This allows it to easily be used with 3v4l (et al) for historical behavior checks

# This data set specifically includes the following features:
#  * referred_by_userid for JOINs and duplicated column name
#  * duplicated record (ignoring userid)
#  * multiple entries using the same country (for FETCH_GROUP and FETCH_UNIQUE)
#  * 2 countries which have the same name (but aren't otherwise duplicates) (for FETCH_GROUP and FETCH_UNIQUE)
#  * names and countries between 4 and 7 chars for nice TSV alignment

if (! isset($table)) {
    die("Must set \$table before including pdo_fetch_setup.php");
}

# SQL Server requires this; Firebird error on it.
$nullable = (($db->getAttribute(PDO::ATTR_DRIVER_NAME) === 'dblib') ? 'NULL' : '');

$db->exec(
    "CREATE TABLE {$table} (
        userid INT PRIMARY KEY,
        name VARCHAR(20),
        country VARCHAR(20),
        referred_by_userid INT {$nullable}
    )"
);

$records = array(
    array(104, 'Chris', 'Ukraine', NULL),
    array(105, 'Jamie', 'England', NULL),
    array(107, 'Robin', 'Germany', 104),
    array(108, 'Sean',  'Ukraine', NULL),
    array(109, 'Toni',  'Germany', NULL),
    array(110, 'Toni',  'Germany', NULL),
    array(111, 'Sean',  'France',  NULL)
);
$insertSql = "INSERT INTO {$table}
        (userid, name, country, referred_by_userid)
        VALUES (:userid, :name, :country, :refid)";
$insert = $db->prepare($insertSql);

foreach ($records as $record) {
    $insert->execute(array(
        'userid' => $record[0],
        'name' => $record[1],
        'country' => $record[2],
        'refid' => $record[3]
    ));
}
