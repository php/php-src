--TEST--
strtotime() function
--FILE--
<?php
date_default_timezone_set('Europe/Oslo');
$d = array();
$d[] = strtotime("2005-07-14 22:30:41");
$d[] = strtotime("2005-07-14 22:30:41 GMT");
$d[] = strtotime("@1121373041");
$d[] = strtotime("@1121373041 CEST");

foreach($d as $date) {
    echo date(DATE_ISO8601, $date), "\n";
}
?>
--EXPECT--
2005-07-14T22:30:41+0200
2005-07-15T00:30:41+0200
2005-07-14T22:30:41+0200
2005-07-14T22:30:41+0200
