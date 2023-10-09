--TEST--
Bug #35517 (mysqli_stmt_fetch returns NULL)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    $mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    $mysql->query("CREATE TABLE temp (id INT UNSIGNED NOT NULL)");
    $mysql->query("INSERT INTO temp (id) VALUES (3000000897),(3800001532),(3900002281),(3100059612)");
    $stmt = $mysql->prepare("SELECT id FROM temp");
    $stmt->execute();
    $stmt->bind_result($id);
    while ($stmt->fetch()) {
        if (PHP_INT_SIZE == 8) {
            if ((gettype($id) !== 'int') && (gettype($id) != 'integer'))
                printf("[001] Expecting integer on 64bit got %s/%s\n", gettype($id), var_export($id, true));
        } else {
            if (gettype($id) !== 'string') {
                printf("[002] Expecting string on 32bit got %s/%s\n", gettype($id), var_export($id, true));
            }
        }
        print $id;
        print "\n";
    }
    $stmt->close();

    $mysql->query("DROP TABLE temp");
    $mysql->close();
    print "done!";
?>
--CLEAN--
<?php
require_once 'connect.inc';
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS temp"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
3000000897
3800001532
3900002281
3100059612
done!
