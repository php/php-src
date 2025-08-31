--TEST--
Bug #75230 (Invalid opcode 49/1/8 using opcache)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function f() {
      $retval = false;
        if ($retval) { }
}
f();
exit("OK");
?>
--EXPECT--
OK
