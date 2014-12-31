--TEST--
Bug #62852 (Unserialize invalid DateTime causes crash), variation 3
--INI--
date.timezone=GMT
--FILE--
<?php
$s2 = 'O:3:"Foo":3:{s:4:"date";s:19:"0000-00-00 00:00:00";s:13:"timezone_type";i:0;s:8:"timezone";s:3:"UTC";}';

global $foo;

class Foo extends DateTime {
    function __wakeup() {
        global $foo;
        $foo = $this;
        parent::__wakeup();
    }
}

try {
    unserialize( $s2 );
} catch ( Exception $e ) {}
var_dump( $foo );

--EXPECTF--
Fatal error: Invalid serialization data for DateTime object in %sbug62852_var3.php on line %d
