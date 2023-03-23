--TEST--
GH-10908 (Bus error with PDO Firebird on RPI with 64 bit kernel and 32 bit userland)
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php

require("testdb.inc");

$sql = <<<EOT
CREATE TABLE gh10908(
  ID BIGINT NOT NULL,
  CODE VARCHAR(60) NOT NULL,
  NUM NUMERIC(18, 3)
);
EOT;
$dbh->exec($sql);
$dbh->exec("INSERT INTO gh10908 VALUES(1, 'ABC', 12.34);");

function query_and_dump($dbh, $sql) {
    foreach ($dbh->query($sql) as $row) {
        print_r($row);
        print("\n");
    }
}

query_and_dump($dbh, "SELECT CODE FROM gh10908"); // works fine
query_and_dump($dbh, "SELECT ID   FROM gh10908"); // Used to "bus error"
query_and_dump($dbh, "SELECT NUM  FROM gh10908"); // Used to "bus error"
query_and_dump($dbh, "SELECT *    FROM gh10908"); // Used to "bus error"

query_and_dump($dbh, "SELECT CAST(NUM AS NUMERIC(9, 3)) FROM gh10908"); // works fine
query_and_dump($dbh, "SELECT CAST(ID AS INTEGER)        FROM gh10908"); // works fine
query_and_dump($dbh, "SELECT CAST(ID AS BIGINT)         FROM gh10908"); // Used to "bus error"

echo "Did not crash\n";

?>
--EXPECT--
Array
(
    [CODE] => ABC
    [0] => ABC
)

Array
(
    [ID] => 1
    [0] => 1
)

Array
(
    [NUM] => 12.340
    [0] => 12.340
)

Array
(
    [ID] => 1
    [0] => 1
    [CODE] => ABC
    [1] => ABC
    [NUM] => 12.340
    [2] => 12.340
)

Array
(
    [CAST] => 12.340
    [0] => 12.340
)

Array
(
    [CAST] => 1
    [0] => 1
)

Array
(
    [CAST] => 1
    [0] => 1
)

Did not crash
--CLEAN--
<?php
require 'testdb.inc';
$dbh->exec("DROP TABLE gh10908");
?>
