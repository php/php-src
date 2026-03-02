--TEST--
NaN handling: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
$b = NAN;
for ($i = 0; $i < 3; $i++) {
    if ($b) { echo "nan is true\n"; }
    else { echo "nan is false\n"; }
}	
?>
--EXPECTF--
Warning: unexpected NAN value was coerced to bool in %s on line 4
nan is true

Warning: unexpected NAN value was coerced to bool in %s on line 4
nan is true

Warning: unexpected NAN value was coerced to bool in %s on line 4
nan is true
