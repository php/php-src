--TEST--
FETCH_OBJ_IS on typed object property
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php

class Test {
    public stdClass $data;
}

function test() {
    $test = new Test;
    var_dump(isset($test->data[0]));
}

test();

?>
--EXPECT--
bool(false)
