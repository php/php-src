--TEST--
Bug #79452 (DateTime::diff() generates months differently between time zones)
--FILE--
<?php
date_default_timezone_set('America/New_York');

$from = new DateTime('2019-06-01');
$to = new DateTime('2019-10-01');

var_dump($from->diff($to)->m);

date_default_timezone_set('Asia/Tokyo');

$from = new DateTime('2019-06-01');
$to = new DateTime('2019-10-01');

var_dump($from->diff($to)->m);
?>
--EXPECT--
int(4)
int(4)
