--TEST--
JIT ADD: 009 two array references
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
$a = []; 
var_dump(($b =& $a) + ($b =& $a));
?>
--EXPECT--
array(0) {
}
