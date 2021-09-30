--TEST--
JIT SEND_VAR_EX may leak with named args
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
for ($i = 0; $i < 3; $i++ ) {
	$x = 0;
	test(a: $x);
	var_dump($x);
}	

function test(&$a = null, SomeType &$b = null) {
    $a++;
}
?>
--EXPECT--
int(1)
int(1)
int(1)
