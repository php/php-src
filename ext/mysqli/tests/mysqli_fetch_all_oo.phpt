--TEST--
$mysqli->fetch_all() (introduced with mysqlnd)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_fetch_all'))
    die("skip: function only available with mysqlnd");
?>
--FILE--
<?php
    require_once("connect.inc");

    $tmp    = NULL;
    $link   = NULL;

    $mysqli = new mysqli();

    if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
        printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    require('table.inc');
    if (!$res = $mysqli->query("SELECT * FROM test ORDER BY id LIMIT 2")) {
        printf("[004] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    print "[005]\n";
    var_dump($res->fetch_all());
    $res->free_result();

    if (!$res = $mysqli->query("SELECT * FROM test ORDER BY id LIMIT 2")) {
        printf("[006] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    print "[007]\n";
    var_dump($res->fetch_all(MYSQLI_NUM));
    $res->free_result();

    if (!$res = $mysqli->query("SELECT * FROM test ORDER BY id LIMIT 2")) {
        printf("[008] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    print "[008]\n";
    var_dump($res->fetch_all(MYSQLI_BOTH));
    $res->free_result();

    if (!$res = $mysqli->query("SELECT * FROM test ORDER BY id LIMIT 2")) {
        printf("[009] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    print "[010]\n";
    var_dump($res->fetch_all(MYSQLI_ASSOC));

    print "[011]\n";
    var_dump($res->fetch_array());
    $res->free_result();

    if (!$res = $mysqli->query("SELECT * FROM test ORDER BY id LIMIT 2")) {
        printf("[012] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    print "[013]\n";
    var_dump($res->fetch_all(MYSQLI_ASSOC));

    print "[016]\n";
    var_dump($res->fetch_array());

    if (!$res = $mysqli->query("SELECT 1 AS a, 2 AS a, 3 AS c, 4 AS C, NULL AS d, true AS e")) {
        printf("[010] Cannot run query, [%d] %s\n", $mysqli->errno, $$mysqli->error);
    }
    print "[017]\n";
    var_dump($res->fetch_all(MYSQLI_BOTH));

    $res->free_result();
    if (!$res = $mysqli->query("SELECT 1 AS a, 2 AS b, 3 AS c, 4 AS C")) {
        printf("[018] Cannot run query, [%d] %s\n",
            $mysqli->errno, $$mysqli->error);
        exit(1);
    }

    try {
        $tmp = $res->fetch_all(-10);
        if (false !== $tmp)
            printf("[019] Expecting boolean/false although, got %s/%s. [%d] %s\n",
                gettype($tmp), $tmp, $mysqli->errno, $mysqli->error);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    $res->free_result();

    function func_mysqli_fetch_all_oo($link, $engine, $sql_type, $sql_value, $php_value, $offset, $regexp_comparison = NULL) {

        if (!$link->query("DROP TABLE IF EXISTS test")) {
            printf("[%04d] [%d] %s\n", $offset, $link->errno, $link->error);
            return false;
        }

        if (!$link->query($sql = sprintf("CREATE TABLE test(id INT NOT NULL, label %s, PRIMARY KEY(id)) ENGINE = %s", $sql_type, $engine))) {
                // don't bail, engine might not support the datatype
                return false;
        }

        if (is_null($php_value)) {
            if (!$link->query($sql = sprintf("INSERT INTO test(id, label) VALUES (1, NULL)"))) {
                printf("[%04d] [%d] %s\n", $offset + 1, $link->errno, $link->error);
                return false;
            }
        } else {
            if (is_string($sql_value)) {
                if (!$link->query($sql = "INSERT INTO test(id, label) VALUES (1, '" . $sql_value . "')")) {
                    printf("[%04ds] [%d] %s - %s\n", $offset + 1, $link->errno, $link->error, $sql);
                    return false;
                }
            } else {
                if (!$link->query($sql = sprintf("INSERT INTO test(id, label) VALUES (1, '%d')", $sql_value))) {
                    printf("[%04di] [%d] %s\n", $offset + 1, $link->errno, $link->error);
                    return false;
                }
            }
        }

        if (!$res = $link->query("SELECT id, label FROM test")) {
            printf("[%04d] [%d] %s\n", $offset + 2, $link->errno, $link->error);
            return false;
        }

        if (!$tmp = $res->fetch_all(MYSQLI_BOTH)) {
            printf("[%04d] [%d] %s\n", $offset + 3, $link->errno, $link->error);
            return false;
        }
        $row = $tmp[0];

        $fields = mysqli_fetch_fields($res);

        if (!(gettype($php_value)=="unicode" && ($fields[1]->flags & 128))) {
            if ($regexp_comparison) {
                if (!preg_match($regexp_comparison, (string)$row['label']) || !preg_match($regexp_comparison, (string)$row[1])) {
                    printf("[%04d] Expecting %s/%s [reg exp = %s], got %s/%s resp. %s/%s. [%d] %s\n", $offset + 4,
                        gettype($php_value), $php_value, $regexp_comparison,
                        gettype($row[1]), $row[1],
                        gettype($row['label']), $row['label'], $link->errno, $link->error);
                    return false;
                }
            } else {
                if (($row['label'] !== $php_value) || ($row[1] != $php_value)) {
                    printf("[%04d] Expecting %s/%s, got %s/%s resp. %s/%s. [%d] %s\n", $offset + 4,
                        gettype($php_value), $php_value,
                        gettype($row[1]), $row[1],
                        gettype($row['label']), $row['label'], $link->errno, $link->error);
                    return false;
                }
            }
        }

        return true;
    }

    function func_mysqli_fetch_array_oo_make_string($len) {

        $ret = '';
        for ($i = 0; $i < $len; $i++)
            $ret .= chr(mt_rand(65, 90));

        return $ret;
    }

    func_mysqli_fetch_all_oo($link, $engine, "TINYINT", -11, "-11", 20);
    func_mysqli_fetch_all_oo($link, $engine, "TINYINT", NULL, NULL, 30);
    func_mysqli_fetch_all_oo($link, $engine, "TINYINT UNSIGNED", 1, "1", 40);
    func_mysqli_fetch_all_oo($link, $engine, "TINYINT UNSIGNED", NULL, NULL, 50);

    func_mysqli_fetch_all_oo($link, $engine, "BOOL", 1, "1", 60);
    func_mysqli_fetch_all_oo($link, $engine, "BOOL", NULL, NULL, 70);
    func_mysqli_fetch_all_oo($link, $engine, "BOOLEAN", 0, "0", 80);
    func_mysqli_fetch_all_oo($link, $engine, "BOOLEAN", NULL, NULL, 90);

    func_mysqli_fetch_all_oo($link, $engine, "SMALLINT", -32768, "-32768", 100);
    func_mysqli_fetch_all_oo($link, $engine, "SMALLINT", 32767, "32767", 110);
    func_mysqli_fetch_all_oo($link, $engine, "SMALLINT", NULL, NULL, 120);
    func_mysqli_fetch_all_oo($link, $engine, "SMALLINT UNSIGNED", 65400, "65400", 130);
    func_mysqli_fetch_all_oo($link, $engine, "SMALLINT UNSIGNED", NULL, NULL, 140);

    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMINT", -8388608, "-8388608", 150);
    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMINT", 8388607, "8388607", 160);
    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMINT", NULL, NULL, 170);
    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMINT UNSIGNED", 16777215, "16777215", 180);
    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMINT UNSIGNED", NULL, NULL, 190);

    func_mysqli_fetch_all_oo($link, $engine, "INTEGER", -2147483648, "-2147483648", 200);
    func_mysqli_fetch_all_oo($link, $engine, "INTEGER", 2147483647, "2147483647", 210);
    func_mysqli_fetch_all_oo($link, $engine, "INTEGER", NULL, NULL, 220);
    func_mysqli_fetch_all_oo($link, $engine, "INTEGER UNSIGNED", "4294967295", "4294967295", 230);
    func_mysqli_fetch_all_oo($link, $engine, "INTEGER UNSIGNED", NULL, NULL, 240);

    func_mysqli_fetch_all_oo($link, $engine, "BIGINT", "-9223372036854775808", "-9223372036854775808", 250);
    func_mysqli_fetch_all_oo($link, $engine, "BIGINT", NULL, NULL, 260);
    func_mysqli_fetch_all_oo($link, $engine, "BIGINT UNSIGNED", "18446744073709551615", "18446744073709551615", 270);
    func_mysqli_fetch_all_oo($link, $engine, "BIGINT UNSIGNED", NULL, NULL, 280);

    func_mysqli_fetch_all_oo($link, $engine, "FLOAT", (string)(-9223372036854775808 - 1.1), "-9.22337e+18", 290, "/-9\.22337e\+?[0]?18/iu");
    func_mysqli_fetch_all_oo($link, $engine, "FLOAT", NULL, NULL, 300);
    func_mysqli_fetch_all_oo($link, $engine, "FLOAT UNSIGNED", (string)(18446744073709551615 + 1.1), "1.84467e+?19", 310, "/1\.84467e\+?[0]?19/iu");
    func_mysqli_fetch_all_oo($link, $engine, "FLOAT UNSIGNED ", NULL, NULL, 320);

    func_mysqli_fetch_all_oo($link, $engine, "DOUBLE(10,2)", "-99999999.99", "-99999999.99", 330);
    func_mysqli_fetch_all_oo($link, $engine, "DOUBLE(10,2)", NULL, NULL, 340);
    func_mysqli_fetch_all_oo($link, $engine, "DOUBLE(10,2) UNSIGNED", "99999999.99", "99999999.99", 350);
    func_mysqli_fetch_all_oo($link, $engine, "DOUBLE(10,2) UNSIGNED", NULL, NULL, 360);

    func_mysqli_fetch_all_oo($link, $engine, "DECIMAL(10,2)", "-99999999.99", "-99999999.99", 370);
    func_mysqli_fetch_all_oo($link, $engine, "DECIMAL(10,2)", NULL, NULL, 380);
    func_mysqli_fetch_all_oo($link, $engine, "DECIMAL(10,2)", "99999999.99", "99999999.99", 390);
    func_mysqli_fetch_all_oo($link, $engine, "DECIMAL(10,2)", NULL, NULL, 400);

    // don't care about date() strict TZ warnings...
    $date = @date('Y-m-d');
    func_mysqli_fetch_all_oo($link, $engine, "DATE", $date, $date, 410);
    func_mysqli_fetch_all_oo($link, $engine, "DATE NOT NULL", $date, $date, 420);
    func_mysqli_fetch_all_oo($link, $engine, "DATE", NULL, NULL, 430);

    $datetime = @date('Y-m-d H:i:s');
    func_mysqli_fetch_all_oo($link, $engine, "DATETIME", $datetime, $datetime, 440);
    func_mysqli_fetch_all_oo($link, $engine, "DATETIME NOT NULL", $datetime, $datetime, 450);
    func_mysqli_fetch_all_oo($link, $engine, "DATETIME", NULL, NULL, 460);

    func_mysqli_fetch_all_oo($link, $engine, "TIMESTAMP", $datetime, $datetime, 470);

    $time = @date('H:i:s');
    func_mysqli_fetch_all_oo($link, $engine, "TIME", $time, $time, 480);
    func_mysqli_fetch_all_oo($link, $engine, "TIME NOT NULL", $time, $time, 490);
    func_mysqli_fetch_all_oo($link, $engine, "TIME", NULL, NULL, 500);

    func_mysqli_fetch_all_oo($link, $engine, "YEAR", @date('Y'), @date('Y'), 510);
    func_mysqli_fetch_all_oo($link, $engine, "YEAR NOT NULL", @date('Y'), @date('Y'), 520);
    func_mysqli_fetch_all_oo($link, $engine, "YEAR", NULL, NULL, 530);

    $string255 = func_mysqli_fetch_array_oo_make_string(255);
    func_mysqli_fetch_all_oo($link, $engine, "CHAR(1)", "a", "a", 540);
    func_mysqli_fetch_all_oo($link, $engine, "CHAR(255)", $string255,  $string255, 550);
    func_mysqli_fetch_all_oo($link, $engine, "CHAR(1) NOT NULL", "a", "a", 560);
    func_mysqli_fetch_all_oo($link, $engine, "CHAR(1)", NULL, NULL, 570);

    $string65k = func_mysqli_fetch_array_oo_make_string(65400);
    func_mysqli_fetch_all_oo($link, $engine, "VARCHAR(1)", "a", "a", 580);
    func_mysqli_fetch_all_oo($link, $engine, "VARCHAR(255)", $string255, $string255, 590);
    func_mysqli_fetch_all_oo($link, $engine, "VARCHAR(65400)", $string65k, $string65k, 600);
    func_mysqli_fetch_all_oo($link, $engine, "VARCHAR(1) NOT NULL", "a", "a", 610);
    func_mysqli_fetch_all_oo($link, $engine, "VARCHAR(1)", NULL, NULL, 620);

    func_mysqli_fetch_all_oo($link, $engine, "BINARY(1)", "a", "a", 630);
    func_mysqli_fetch_all_oo($link, $engine, "BINARY(2)", chr(0) . "a", chr(0) . "a", 640);
    func_mysqli_fetch_all_oo($link, $engine, "BINARY(1) NOT NULL", "b", "b", 650);
    func_mysqli_fetch_all_oo($link, $engine, "BINARY(1)", NULL, NULL, 660);

    func_mysqli_fetch_all_oo($link, $engine, "VARBINARY(1)", "a", "a", 670);
    func_mysqli_fetch_all_oo($link, $engine, "VARBINARY(2)", chr(0) . "a", chr(0) . "a", 680);
    func_mysqli_fetch_all_oo($link, $engine, "VARBINARY(1) NOT NULL", "b", "b", 690);
    func_mysqli_fetch_all_oo($link, $engine, "VARBINARY(1)", NULL, NULL, 700);

    func_mysqli_fetch_all_oo($link, $engine, "TINYBLOB", "a", "a", 710);
    func_mysqli_fetch_all_oo($link, $engine, "TINYBLOB", chr(0) . "a", chr(0) . "a", 720);
    func_mysqli_fetch_all_oo($link, $engine, "TINYBLOB NOT NULL", "b", "b", 730);
    func_mysqli_fetch_all_oo($link, $engine, "TINYBLOB", NULL, NULL, 740);

    func_mysqli_fetch_all_oo($link, $engine, "TINYTEXT", "a", "a", 750);
    func_mysqli_fetch_all_oo($link, $engine, "TINYTEXT NOT NULL", "a", "a", 760);
    func_mysqli_fetch_all_oo($link, $engine, "TINYTEXT", NULL, NULL, 770);

    func_mysqli_fetch_all_oo($link, $engine, "BLOB", "a", "a", 780);
    func_mysqli_fetch_all_oo($link, $engine, "BLOB", chr(0) . "a", chr(0) . "a", 780);
    func_mysqli_fetch_all_oo($link, $engine, "BLOB", NULL, NULL, 790);

    func_mysqli_fetch_all_oo($link, $engine, "TEXT", "a", "a", 800);
    func_mysqli_fetch_all_oo($link, $engine, "TEXT", chr(0) . "a", chr(0) . "a", 810);
    func_mysqli_fetch_all_oo($link, $engine, "TEXT", NULL, NULL, 820);

    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMBLOB", "a", "a", 830);
    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMBLOB", chr(0) . "a", chr(0) . "a", 840);
    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMBLOB", NULL, NULL, 850);

    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMTEXT", "a", "a", 860);
    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMTEXT", chr(0) . "a", chr(0) . "a", 870);
    func_mysqli_fetch_all_oo($link, $engine, "MEDIUMTEXT", NULL, NULL, 880);

    func_mysqli_fetch_all_oo($link, $engine, "LONGBLOB", "a", "a", 890);
    func_mysqli_fetch_all_oo($link, $engine, "LONGTEXT", chr(0) . "a", chr(0) . "a", 900);
    func_mysqli_fetch_all_oo($link, $engine, "LONGBLOB", NULL, NULL, 910);

    func_mysqli_fetch_all_oo($link, $engine, "ENUM('a', 'b')", "a", "a", 920);
    func_mysqli_fetch_all_oo($link, $engine, "ENUM('a', 'b')", NULL, NULL, 930);

    func_mysqli_fetch_all_oo($link, $engine, "SET('a', 'b')", "a", "a", 940);
    func_mysqli_fetch_all_oo($link, $engine, "SET('a', 'b')", NULL, NULL, 950);

    mysqli_close($link);

    try {
        $res->fetch_array(MYSQLI_ASSOC);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
[005]
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(1) "b"
  }
}
[007]
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(1) "b"
  }
}
[008]
array(2) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "1"
    ["id"]=>
    string(1) "1"
    [1]=>
    string(1) "a"
    ["label"]=>
    string(1) "a"
  }
  [1]=>
  array(4) {
    [0]=>
    string(1) "2"
    ["id"]=>
    string(1) "2"
    [1]=>
    string(1) "b"
    ["label"]=>
    string(1) "b"
  }
}
[010]
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    ["label"]=>
    string(1) "b"
  }
}
[011]
NULL
[013]
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    ["label"]=>
    string(1) "b"
  }
}
[016]
NULL
[017]
array(1) {
  [0]=>
  array(11) {
    [0]=>
    string(1) "1"
    ["a"]=>
    string(1) "2"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    ["c"]=>
    string(1) "3"
    [3]=>
    string(1) "4"
    ["C"]=>
    string(1) "4"
    [4]=>
    NULL
    ["d"]=>
    NULL
    [5]=>
    string(1) "1"
    ["e"]=>
    string(1) "1"
  }
}
mysqli_result::fetch_all(): Argument #1 ($mode) must be one of MYSQLI_FETCH_NUM, MYSQLI_FETCH_ASSOC, or MYSQLI_FETCH_BOTH
mysqli_result object is already closed
done!
