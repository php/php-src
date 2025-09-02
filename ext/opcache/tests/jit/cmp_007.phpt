--TEST--
JIT CMP: 007 Wrong comparison skip
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
function test($a) {
    var_dump(1 - $a != 0);
}
for ($i = 0; $i < 5; $i++) {
	test(null);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
