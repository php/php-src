--TEST--
Bind static deoptimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test($n) {
    static $x = array(A);
	return isset($x[$n]); 
}
define("A", 42);
for ($i = 0; $i < 5; $i++) {
	var_dump(test(0));
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
