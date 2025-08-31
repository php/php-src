--TEST--
function test: mysqli_num_fields()
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

    mysqli_query($link, "CREATE TABLE test_mysqli_num_fields (a int, b varchar(10)) ENGINE = " . get_default_db_engine());

    mysqli_query($link, "INSERT INTO test_mysqli_num_fields VALUES (1, 'foo')");

    mysqli_real_query($link, "SELECT * FROM test_mysqli_num_fields");
    if (mysqli_field_count($link)) {
        $result = mysqli_store_result($link);
        $num = mysqli_num_fields($result);
        mysqli_free_result($result);
    }

    var_dump($num);

    mysqli_close($link);
?>
--CLEAN--
<?php
require_once(dirname(__DIR__) . "/test_setup/test_helpers.inc");
tear_down_table_on_default_connection('test_mysqli_num_fields');
?>
--EXPECT--
int(2)
