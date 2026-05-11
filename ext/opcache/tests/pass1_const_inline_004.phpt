--TEST--
define() followed by const redeclaration is not inlined
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
--FILE--
<?php

define('FOO', 'first');
const FOO = 'second';

function get_foo() {
    return FOO;
}

var_dump(get_foo());

?>
--EXPECTF--
Warning: Constant FOO already defined, this will be an error in PHP 9 in %s on line %d
string(5) "first"
