--TEST--
GH-13232 (Segmentation fault will be reported when JIT is off but JIT_debug is still on)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_debug=0xfffff
--FILE--
<?php
echo "Done\n";
?>
--EXPECT--
Done
