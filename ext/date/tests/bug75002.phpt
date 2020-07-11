--TEST--
Bug #75002 Null Pointer Dereference in timelib_time_clone
--FILE--
<?php

class aaa extends DatePeriod {
    public function __construct() { }
}

$start=new DateTime( '2012-08-01' );

foreach (new aaa($start) as $y) {
    $a=$key;
}

?>
==DONE==
--EXPECTF--
Fatal error: Uncaught Error: DatePeriod has not been initialized correctly in %sbug75002.php:%d
Stack trace:
#0 {main}
  thrown in %sbug75002.php on line %d
