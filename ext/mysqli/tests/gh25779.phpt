--TEST--
GH-25779: mysqli_fetch_object() crashes with JIT when the constructor keeps a reference and the result is discarded
--EXTENSIONS--
mysqli
opcache
--SKIPIF--
<?php
require_once __DIR__ . '/test_setup/test_helpers.inc';
mysqli_check_skip_test();
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--FILE--
<?php
require_once __DIR__ . '/test_setup/test_helpers.inc';

class Row {
    public static array $rows = [];

    public function __construct() {
        self::$rows[] = $this;
    }

    public function __destruct() { echo "destruct\n"; }
}

function test(mysqli_result $result) {
    mysqli_fetch_object($result, Row::class);
}

$link = default_mysqli_connect();
test(mysqli_query($link, "SELECT 1 AS id"));
var_dump(Row::$rows);
?>
--EXPECTF--
array(1) {
  [0]=>
  object(Row)#%d (1) {
    ["id"]=>
    string(1) "1"
  }
}
destruct
