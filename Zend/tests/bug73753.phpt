--TEST--
Bug #73753 Non packed arrays and duplication
--FILE--
<?php
function iterate($current, $a, $result = null) {
    if (!$current) {
        return $result;
    }

    return iterate(getNext($a), $a, $current);
}

function getNext(&$a) {
    return next($a);
}

function getCurrent($a) {
    return current($a);
}

function traverse($a) {
    return iterate(getCurrent($a), $a);
}

$arr = array(1 => 'foo', 'b' => 'bar', 'baz');
var_dump(traverse($arr));
?>
--EXPECTF--
string(3) "baz"
