--TEST--
Bug #80194: Assertion failure during block assembly of unreachable free with leading nop
--FILE--
<?php

function test($x) {
    switch ($x->y) {
        default:
            throw new Exception;
        case 'foobar':
            return new stdClass();
            break;
    }
}

$x = (object)['y' => 'foobar'];
var_dump(test($x));

?>
--EXPECT--
object(stdClass)#2 (0) {
}
