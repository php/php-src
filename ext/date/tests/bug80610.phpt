--TEST--
Bug #80610 (DateTime calculate wrong with DateInterval)
--FILE--
<?php
$expectEaster = date_create_from_format('Y-m-d H:i:s', '2020-04-12 12:00:00', new DateTimeZone('Europe/Berlin'));
$interval = new DateInterval('PT20800M');
$expectEaster->sub($interval);
$expectEaster->add($interval);
echo('easter '.$expectEaster->format('Y-m-d H:i:s')."\n" );

$expectEaster = date_create_from_format('Y-m-d H:i:s', '2020-04-12 12:00:00', new DateTimeZone('Europe/Berlin'));
$interval = new DateInterval('PT20715M');
$expectEaster->sub($interval);
$expectEaster->add($interval);
echo('easter '.$expectEaster->format('Y-m-d H:i:s')."\n");

$expectEaster = date_create_from_format('Y-m-d H:i:s', '2020-04-12 12:00:00', new DateTimeZone('Europe/Berlin'));
$interval = new DateInterval('PT20700M');
$expectEaster->sub($interval);
$expectEaster->add($interval);
echo('easter '.$expectEaster->format('Y-m-d H:i:s')."\n");
?>
--EXPECT--
easter 2020-04-12 12:00:00
easter 2020-04-12 12:00:00
easter 2020-04-12 12:00:00
