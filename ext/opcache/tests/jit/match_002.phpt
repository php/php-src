--TEST--
JIT Match: 002 impossible input
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
class C {
}
function foo(C $i) {
    return match($i) {
    	1,2=>0,
    	default=>42
    };
};
var_dump(foo(new C));
?>
--EXPECT--
int(42)
