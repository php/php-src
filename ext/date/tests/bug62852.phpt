--TEST--
Bug #62852 (Unserialize invalid DateTime causes crash)
--INI--
date.timezone=GMT
--XFAIL--
bug is not fixed yet
--FILE--
<?php
$s1 = 'O:8:"DateTime":3:{s:4:"date";s:20:"10007-06-07 03:51:49";s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}';
$s2 = 'O:3:"Foo":3:{s:4:"date";s:20:"10007-06-07 03:51:49";s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}';

global $foo;

class Foo extends DateTime {
    function __wakeup() {
        global $foo;
        $foo = $this;
        parent::__wakeup();
    }
}

// Old test case
try {
    unserialize( $s1 );
} catch ( Exception $e ) {}

// My test case
try {
    unserialize( $s2 );
} catch ( Exception $e ) {}
var_dump( $foo );

echo "okey";
?>
--EXPECTF--
okey
