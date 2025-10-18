<?php
if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR='. __DIR__ . '/../../../pdo/tests/');
}
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

error_reporting(E_ALL);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

# Note: code below is written to be compatible in PHP 5.1+ (and sqlite of that era) (when PDO was introduced)
# This allows it to easily be used with 3v4l (et al) for historical behavior checks

# This data set specifically includes the following features:
#  * referred_by_userid for JOINs and duplicated column name
#  * duplicated record (ignoring userid)
#  * multiple entries using the same country (for FETCH_GROUP and FETCH_UNIQUE)
#  * 2 countries which have the same name (but aren't otherwise duplicates) (for FETCH_GROUP and FETCH_UNIQUE)
#  * names and countries between 4 and 7 chars for nice TSV alignment

$db->exec(
    "CREATE TABLE users (
        userid INT PRIMARY KEY,
        name TEXT,
        country TEXT,
        referred_by_userid INT
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
$insertSql = "INSERT INTO users
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
