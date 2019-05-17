--TEST--
Bug #62852 (Unserialize invalid DateTime causes crash), variation 2
--INI--
date.timezone=GMT
--FILE--
<?php
$s2 = 'O:3:"Foo":3:{s:4:"date";s:20:"10007-06-07 03:51:49";s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}';

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
Fatal error: Uncaught Error: Invalid serialization data for DateTime object in %sbug62852_var2.php:%d
Stack trace:
#0 %sbug62852_var2.php(%d): DateTime->__wakeup()
#1 [internal function]: Foo->__wakeup()
#2 %sbug62852_var2.php(%d): unserialize('O:3:"Foo":3:{s:...')
#3 {main}
  thrown in %sbug62852_var2.php on line %d
