--TEST--
ISSET_ISEMPTY_DIM with undefined variable
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function test() {
    var_dump(isset($a[$undef]));
}
test();
?>
--EXPECTF--
Warning: Undefined variable $undef in %s on line %d
bool(false)
