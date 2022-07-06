--TEST--
Bug #51393 (DateTime::createFromFormat() fails if format string contains timezone)
--INI--
date.timezone=UTC
--FILE--
<?php
$dt = DateTime::createFromFormat('O', '+0800');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('P', '+08:00');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('O', '-0800');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('P', '-08:00');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('[O]', '[+0800]');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('[P]', '[+08:00]');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('[O]', '[-0800]');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('[P]', '[-08:00]');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('O', 'GMT+0800');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('P', 'GMT+08:00');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('O', 'GMT-0800');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('P', 'GMT-08:00');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('[O]', '[GMT+0800]');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('[P]', '[GMT+08:00]');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('[O]', '[GMT-0800]');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('[P]', '[GMT-08:00]');
var_dump($dt->getOffset());

$dt = DateTime::createFromFormat('O', 'invalid');
var_dump($dt);
?>
--EXPECT--
int(28800)
int(28800)
int(-28800)
int(-28800)
int(28800)
int(28800)
int(-28800)
int(-28800)
int(28800)
int(28800)
int(-28800)
int(-28800)
int(28800)
int(28800)
int(-28800)
int(-28800)
bool(false)
