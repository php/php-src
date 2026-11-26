--TEST--
GH-25772: pg_fetch_object() crashes with JIT when the constructor keeps a reference and the result is discarded
--EXTENSIONS--
pgsql
opcache
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--FILE--
<?php
include("inc/config.inc");

class Row {
    public static array $rows = [];

    public function __construct() {
        self::$rows[] = $this;
    }
}

function test(PgSql\Result $result) {
    pg_fetch_object($result, 0, Row::class);
}

$conn = pg_connect($conn_str);
test(pg_query($conn, "SELECT 1 AS id"));
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
