--TEST--
Bug #75893: file_get_contents $http_response_header variable bugged with opcache
--INI--
opcache.enable_cli=1
track_errors=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function test() {
    echo $undef;
    $foo = $php_errormsg;
    var_dump($foo[0]);
}

test();

?>
--EXPECTF--
Deprecated: Directive 'track_errors' is deprecated in Unknown on line 0

Notice: Undefined variable: undef in %s on line %d
string(1) "U"
