--TEST--
JIT ASSIGN_DIM: 013
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function test() {
    for(;;) {
        $arr[] = $obj = new stdClass;
        $obj->y = set_error_handler(function(){});
        foreach($obj as $y) {
        }
        $arr = ['' => y];
    }
}
test();
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "y" in %sassign_dim_013.php:8
Stack trace:
#0 %sassign_dim_013.php(11): test()
#1 {main}
  thrown in %sassign_dim_013.php on line 8