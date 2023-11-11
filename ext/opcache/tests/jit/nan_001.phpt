--TEST--
NaN handling: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--FILE--
<?php
$b = NAN;
for ($i = 0; $i < 3; $i++) {
    if ($b) { echo "nan is true\n"; }
    else { echo "nan is false\n"; }
}	
?>
--EXPECT--
nan is true
nan is true
nan is true
