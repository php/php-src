--TEST--
GH-25788: clone() with properties corrupts memory with JIT when __clone() keeps a reference and the result is discarded
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--FILE--
<?php

class Row {
    public static array $rows = [];

    public int $id = 1;

    public function __clone() {
        self::$rows[] = $this;
    }

    public function __destruct() { echo "destruct\n"; }
}

function test(Row $row) {
    clone($row, ['id' => 2]);
}

test(new Row());
var_dump(Row::$rows);
?>
--EXPECTF--
destruct
array(1) {
  [0]=>
  object(Row)#%d (1) {
    ["id"]=>
    int(2)
  }
}
destruct
