--TEST--
SCCP: isset()/empty() on dimension of non-escaping object must not be evaluated
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
class P {}

function f() {
    $o = new P;
    return isset($o[1]);
}

function g() {
    $o = new P;
    return empty($o[1]);
}

try {
    var_dump(f());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(g());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot use object of type P as array
Cannot use object of type P as array
