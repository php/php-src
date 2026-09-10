--TEST--
GH-23644: Crash on a constant-vs-constant empty array comparison
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function f($x) {
    if (null !== $x || [] !== $x) { return 1; }
    return 2;
}

function g($x) {
    if ($x === null) { return $x === [] ? 'eq' : 'ne'; }
    return 'other';
}

var_dump(f(null));
var_dump(g(null));
?>
--EXPECT--
int(1)
string(2) "ne"
