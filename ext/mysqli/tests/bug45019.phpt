--TEST--
Bug #45019 (Segmentation fault with SELECT ? and UNION)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");
    require_once("table.inc");

    // Regular (non-prepared) queries
    print "Using CAST('somestring' AS CHAR)...\n";
    if (!($res = $link->query("SELECT CAST('one' AS CHAR) AS column1 UNION SELECT CAST('three' AS CHAR) UNION SELECT CAST('two' AS CHAR)")))
        printf("[001] [%d] %s\n", $link->errno, $link->error);

    $data = array();
    while ($row = $res->fetch_assoc()) {
        $data[] = $row['column1'];
        var_dump($row['column1']);
    }
    $res->free();

    // Prepared Statements
    if (!($stmt = $link->prepare("SELECT CAST('one' AS CHAR) AS column1 UNION SELECT CAST('three' AS CHAR) UNION SELECT CAST('two' AS CHAR)")))
        printf("[002] [%d] %s\n", $link->errno, $link->error);

    $column1 = null;
    if (!$stmt->bind_result($column1) || !$stmt->execute())
        printf("[003] [%d] %s\n", $stmt->errno, $stmt->error);

    $index = 0;
    while ($stmt->fetch()) {
        if ($data[$index] != $column1) {
            printf("[004] Row %d, expecting %s/%s got %s/%s\n",
                $index + 1, gettype($data[$index]), $data[$index], gettype($column1), $column1);
        }
        $index++;
    }
    $stmt->close();

    $link->close();

    print "done!";
?>
--EXPECT--
Using CAST('somestring' AS CHAR)...
string(3) "one"
string(5) "three"
string(3) "two"
done!
