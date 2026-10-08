--TEST--
Assignment through typed references must not reuse the assigned range
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
class Box {
    public int $value = 0;
}

function assignBounded(&$reference, $value) {
    if ($value > 5 && $value < 7) {
        $result = ($reference = $value);
        if (is_int($result)) {
            return $result;
        }
    }
}

function assignEqual(&$reference, $value) {
    if ($value == 6) {
        $result = ($reference = $value);
        if (is_int($result)) {
            return $result;
        }
    }
}

function assignGlobal($value) {
    if ($value > 5 && $value < 7) {
        $result = ($GLOBALS['reference'] = $value);
        if (is_int($result)) {
            return $result;
        }
    }
}

$box = new Box();
echo 'float: ', assignBounded($box->value, 5.5), ', stored: ', $box->value, "\n";
echo 'numeric string: ', assignBounded($box->value, '5.5'), ', stored: ', $box->value, "\n";
echo 'boolean: ', assignEqual($box->value, true), ', stored: ', $box->value, "\n";
echo 'integer: ', assignBounded($box->value, 6), ', stored: ', $box->value, "\n";

$GLOBALS['reference'] = &$box->value;
echo 'global float: ', assignGlobal(5.5), ', stored: ', $box->value, "\n";
echo 'global integer: ', assignGlobal(6), ', stored: ', $box->value, "\n";
?>
--EXPECT--
float: 5, stored: 5
numeric string: 5, stored: 5
boolean: 1, stored: 1
integer: 6, stored: 6
global float: 5, stored: 5
global integer: 6, stored: 6
