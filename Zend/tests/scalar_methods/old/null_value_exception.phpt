--TEST--
Using __callStatic on null for NullValueException
--FILE--
<?php

use extension null NullHandler;

class NullValueException extends Exception {

}

class NullHandler {
    public static function __callStatic($name, $args) {
        throw new NullValueException('Calling '.$name.' on null');
    }
}

$null= null;
try {
    $null->method();
} catch (NullValueException $expected) {
    echo "Caught expected NVE '", $expected->getMessage(), "'\n";
}

?>
--EXPECTF--
Caught expected NVE 'Calling method on null'
