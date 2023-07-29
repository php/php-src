--TEST--
mysqli_fetch_all() data types variation
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
mysqli_check_skip_test();
?>
--FILE--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";

$link = default_mysqli_connect();

function func_mysqli_fetch_all(
    mysqli $link,
    string $engine,
    string $sql_type,
    string|int|null $sql_value,
    mixed $php_value,
    $offset,
    $regexp_comparison = NULL
) {
    try {
        mysqli_query($link, $sql = sprintf("CREATE TABLE test_mysqli_fetch_all_data_types_variation(id INT NOT NULL, label %s, PRIMARY KEY(id)) ENGINE = %s", $sql_type, $engine));
    } catch (mysqli_sql_exception $e) {
        // don't bail, engine might not support the datatype
        echo  $e->getMessage(), PHP_EOL;
        return;
    }

    if (is_null($php_value)) {
        mysqli_query($link, $sql = sprintf("INSERT INTO test_mysqli_fetch_all_data_types_variation(id, label) VALUES (1, NULL)"));
    } else {
        if (is_string($sql_value)) {
            mysqli_query($link, $sql = "INSERT INTO test_mysqli_fetch_all_data_types_variation(id, label) VALUES (1, '" . $sql_value . "')");
        } else {
            mysqli_query($link, $sql = sprintf("INSERT INTO test_mysqli_fetch_all_data_types_variation(id, label) VALUES (1, '%d')", $sql_value));
        }
    }

    $result = mysqli_query($link, "SELECT id, label FROM test_mysqli_fetch_all_data_types_variation");
    $tmp = mysqli_fetch_all($result, MYSQLI_BOTH);
    $row = $tmp[0];

    $fields = mysqli_fetch_fields($result);

    // I have no idea what this condition does as this seem to be some PHP 6 stuff???
    // if (!(gettype($php_value)=="unicode" && ($fields[1]->flags & 128))) {

    if ($regexp_comparison) {
        if (!preg_match($regexp_comparison, (string)$row['label']) || !preg_match($regexp_comparison, (string)$row[1])) {
            printf("[%04d] Expecting %s/%s [reg exp = %s], got %s/%s resp. %s/%s. [%d] %s\n", $offset + 4,
                gettype($php_value), $php_value, $regexp_comparison,
                gettype($row[1]), $row[1],
                gettype($row['label']), $row['label'], mysqli_errno($link), mysqli_error($link));
        }
    } else {
        if (($row['label'] !== $php_value) || ($row[1] != $php_value)) {
            printf("[%04d] Expecting %s/%s, got %s/%s resp. %s/%s. [%d] %s\n", $offset + 4,
                gettype($php_value), $php_value,
                gettype($row[1]), $row[1],
                gettype($row['label']), $row['label'], mysqli_errno($link), mysqli_error($link));
        }
    }
    mysqli_query($link, 'DROP TABLE test_mysqli_fetch_all_data_types_variation');
}

// TODO Optimize this?
function func_mysqli_fetch_array_make_string(int $len): string {
    $ret = '';
    for ($i = 0; $i < $len; $i++) {
        $ret .= chr(mt_rand(65, 90));
    }
    return $ret;
}

$engine = get_default_db_engine();
func_mysqli_fetch_all($link, $engine, "TINYINT", -11, "-11", 20);
func_mysqli_fetch_all($link, $engine, "TINYINT", NULL, NULL, 30);
func_mysqli_fetch_all($link, $engine, "TINYINT UNSIGNED", 1, "1", 40);
func_mysqli_fetch_all($link, $engine, "TINYINT UNSIGNED", NULL, NULL, 50);

func_mysqli_fetch_all($link, $engine, "BOOL", 1, "1", 60);
func_mysqli_fetch_all($link, $engine, "BOOL", NULL, NULL, 70);
func_mysqli_fetch_all($link, $engine, "BOOLEAN", 0, "0", 80);
func_mysqli_fetch_all($link, $engine, "BOOLEAN", NULL, NULL, 90);

func_mysqli_fetch_all($link, $engine, "SMALLINT", -32768, "-32768", 100);
func_mysqli_fetch_all($link, $engine, "SMALLINT", 32767, "32767", 110);
func_mysqli_fetch_all($link, $engine, "SMALLINT", NULL, NULL, 120);
func_mysqli_fetch_all($link, $engine, "SMALLINT UNSIGNED", 65535, "65535", 130);
func_mysqli_fetch_all($link, $engine, "SMALLINT UNSIGNED", NULL, NULL, 140);

func_mysqli_fetch_all($link, $engine, "MEDIUMINT", -8388608, "-8388608", 150);
func_mysqli_fetch_all($link, $engine, "MEDIUMINT", 8388607, "8388607", 160);
func_mysqli_fetch_all($link, $engine, "MEDIUMINT", NULL, NULL, 170);
func_mysqli_fetch_all($link, $engine, "MEDIUMINT UNSIGNED", 16777215, "16777215", 180);
func_mysqli_fetch_all($link, $engine, "MEDIUMINT UNSIGNED", NULL, NULL, 190);

func_mysqli_fetch_all($link, $engine, "INTEGER", -2147483648, "-2147483648", 200);
func_mysqli_fetch_all($link, $engine, "INTEGER", 2147483647, "2147483647", 210);
func_mysqli_fetch_all($link, $engine, "INTEGER", NULL, NULL, 220);
func_mysqli_fetch_all($link, $engine, "INTEGER UNSIGNED", "4294967295", "4294967295", 230);
func_mysqli_fetch_all($link, $engine, "INTEGER UNSIGNED", NULL, NULL, 240);

func_mysqli_fetch_all($link, $engine, "BIGINT", "-9223372036854775808", "-9223372036854775808", 250);

func_mysqli_fetch_all($link, $engine, "BIGINT", NULL, NULL, 260);
func_mysqli_fetch_all($link, $engine, "BIGINT UNSIGNED", "18446744073709551615", "18446744073709551615", 270);
func_mysqli_fetch_all($link, $engine, "BIGINT UNSIGNED", NULL, NULL, 280);

func_mysqli_fetch_all($link, $engine, "FLOAT", (string)(-9223372036854775808 - 1.1), "-9.22337e+18", 290, "/-9\.22337e\+?[0]?18/iu");
func_mysqli_fetch_all($link, $engine, "FLOAT", NULL, NULL, 300);
func_mysqli_fetch_all($link, $engine, "FLOAT UNSIGNED", (string)(18446744073709551615 + 1.1), "1.84467e+19", 310, "/1\.84467e\+?[0]?19/iu");
func_mysqli_fetch_all($link, $engine, "FLOAT UNSIGNED ", NULL, NULL, 320);

func_mysqli_fetch_all($link, $engine, "DOUBLE(10,2)", "-99999999.99", "-99999999.99", 330);
func_mysqli_fetch_all($link, $engine, "DOUBLE(10,2)", NULL, NULL, 340);
func_mysqli_fetch_all($link, $engine, "DOUBLE(10,2) UNSIGNED", "99999999.99", "99999999.99", 350);
func_mysqli_fetch_all($link, $engine, "DOUBLE(10,2) UNSIGNED", NULL, NULL, 360);

func_mysqli_fetch_all($link, $engine, "DECIMAL(10,2)", "-99999999.99", "-99999999.99", 370);
func_mysqli_fetch_all($link, $engine, "DECIMAL(10,2)", NULL, NULL, 380);
func_mysqli_fetch_all($link, $engine, "DECIMAL(10,2)", "99999999.99", "99999999.99", 390);
func_mysqli_fetch_all($link, $engine, "DECIMAL(10,2)", NULL, NULL, 400);

// don't care about date() strict TZ warnings...
func_mysqli_fetch_all($link, $engine, "DATE", @date('Y-m-d'), @date('Y-m-d'), 410);
func_mysqli_fetch_all($link, $engine, "DATE NOT NULL", @date('Y-m-d'), @date('Y-m-d'), 420);
func_mysqli_fetch_all($link, $engine, "DATE", NULL, NULL, 430);

func_mysqli_fetch_all($link, $engine, "DATETIME", @date('Y-m-d H:i:s'), @date('Y-m-d H:i:s'), 440);
func_mysqli_fetch_all($link, $engine, "DATETIME NOT NULL", @date('Y-m-d H:i:s'), @date('Y-m-d H:i:s'), 450);
func_mysqli_fetch_all($link, $engine, "DATETIME", NULL, NULL, 460);

func_mysqli_fetch_all($link, $engine, "TIMESTAMP", @date('Y-m-d H:i:s'), @date('Y-m-d H:i:s'), 470);

func_mysqli_fetch_all($link, $engine, "TIME", @date('H:i:s'), @date('H:i:s'), 480);
func_mysqli_fetch_all($link, $engine, "TIME NOT NULL", @date('H:i:s'), @date('H:i:s'), 490);
func_mysqli_fetch_all($link, $engine, "TIME", NULL, NULL, 500);

func_mysqli_fetch_all($link, $engine, "YEAR", @date('Y'), @date('Y'), 510);
func_mysqli_fetch_all($link, $engine, "YEAR NOT NULL", @date('Y'), @date('Y'), 520);
func_mysqli_fetch_all($link, $engine, "YEAR", NULL, NULL, 530);

$string255 = func_mysqli_fetch_array_make_string(255);
func_mysqli_fetch_all($link, $engine, "CHAR(1)", "a", "a", 540);
func_mysqli_fetch_all($link, $engine, "CHAR(255)", $string255,  $string255, 550);
func_mysqli_fetch_all($link, $engine, "CHAR(1) NOT NULL", "a", "a", 560);
func_mysqli_fetch_all($link, $engine, "CHAR(1)", NULL, NULL, 570);

$string65k = func_mysqli_fetch_array_make_string(65400);
func_mysqli_fetch_all($link, $engine, "VARCHAR(1)", "a", "a", 580);
func_mysqli_fetch_all($link, $engine, "VARCHAR(255)", $string255, $string255, 590);
func_mysqli_fetch_all($link, $engine, "VARCHAR(65400)", $string65k, $string65k, 600);
func_mysqli_fetch_all($link, $engine, "VARCHAR(1) NOT NULL", "a", "a", 610);
func_mysqli_fetch_all($link, $engine, "VARCHAR(1)", NULL, NULL, 620);

func_mysqli_fetch_all($link, $engine, "BINARY(1)", "a", "a", 630);
func_mysqli_fetch_all($link, $engine, "BINARY(2)", chr(0) . "a", chr(0) . "a", 640);
func_mysqli_fetch_all($link, $engine, "BINARY(1) NOT NULL", "b", "b", 650);
func_mysqli_fetch_all($link, $engine, "BINARY(1)", NULL, NULL, 660);

func_mysqli_fetch_all($link, $engine, "VARBINARY(1)", "a", "a", 670);
func_mysqli_fetch_all($link, $engine, "VARBINARY(2)", chr(0) . "a", chr(0) . "a", 680);
func_mysqli_fetch_all($link, $engine, "VARBINARY(1) NOT NULL", "b", "b", 690);
func_mysqli_fetch_all($link, $engine, "VARBINARY(1)", NULL, NULL, 700);

func_mysqli_fetch_all($link, $engine, "TINYBLOB", "a", "a", 710);
func_mysqli_fetch_all($link, $engine, "TINYBLOB", chr(0) . "a", chr(0) . "a", 720);
func_mysqli_fetch_all($link, $engine, "TINYBLOB NOT NULL", "b", "b", 730);
func_mysqli_fetch_all($link, $engine, "TINYBLOB", NULL, NULL, 740);

func_mysqli_fetch_all($link, $engine, "TINYTEXT", "a", "a", 750);
func_mysqli_fetch_all($link, $engine, "TINYTEXT NOT NULL", "a", "a", 760);
func_mysqli_fetch_all($link, $engine, "TINYTEXT", NULL, NULL, 770);

func_mysqli_fetch_all($link, $engine, "BLOB", "a", "a", 780);
func_mysqli_fetch_all($link, $engine, "BLOB", chr(0) . "a", chr(0) . "a", 780);
func_mysqli_fetch_all($link, $engine, "BLOB", NULL, NULL, 790);

func_mysqli_fetch_all($link, $engine, "TEXT", "a", "a", 800);
func_mysqli_fetch_all($link, $engine, "TEXT", chr(0) . "a", chr(0) . "a", 810);
func_mysqli_fetch_all($link, $engine, "TEXT", NULL, NULL, 820);

func_mysqli_fetch_all($link, $engine, "MEDIUMBLOB", "a", "a", 830);
func_mysqli_fetch_all($link, $engine, "MEDIUMBLOB", chr(0) . "a", chr(0) . "a", 840);
func_mysqli_fetch_all($link, $engine, "MEDIUMBLOB", NULL, NULL, 850);

func_mysqli_fetch_all($link, $engine, "MEDIUMTEXT", "a", "a", 860);
func_mysqli_fetch_all($link, $engine, "MEDIUMTEXT", chr(0) . "a", chr(0) . "a", 870);
func_mysqli_fetch_all($link, $engine, "MEDIUMTEXT", NULL, NULL, 880);

func_mysqli_fetch_all($link, $engine, "LONGBLOB", "a", "a", 890);
func_mysqli_fetch_all($link, $engine, "LONGTEXT", chr(0) . "a", chr(0) . "a", 900);
func_mysqli_fetch_all($link, $engine, "LONGBLOB", NULL, NULL, 910);

func_mysqli_fetch_all($link, $engine, "ENUM('a', 'b')", "a", "a", 920);
func_mysqli_fetch_all($link, $engine, "ENUM('a', 'b')", NULL, NULL, 930);

func_mysqli_fetch_all($link, $engine, "SET('a', 'b')", "a", "a", 940);
func_mysqli_fetch_all($link, $engine, "SET('a', 'b')", NULL, NULL, 950);

mysqli_close($link);

// Split below into new test?
$link = default_mysqli_connect();
mysqli_real_query($link, "SELECT 1 AS _one");
$result = mysqli_use_result($link);

/* on mysqlnd level this would not be allowed */
if (!is_object($result)) {
    printf("[018] Expecting object, got %s/%s. [%d] %s\n",
        gettype($result), $result, mysqli_errno($link), mysqli_error($link));

}

$rows = mysqli_fetch_all($result, MYSQLI_ASSOC);
if (!is_array($rows) || (count($rows) > 1) || !isset($rows[0]['_one']) || ($rows[0]['_one'] != 1)) {
    printf("[019] Results seem wrong, dumping\n");
    var_dump($rows);
}

print "OK";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_mysqli_fetch_all_data_types_variation');
?>
--EXPECT--
OK
