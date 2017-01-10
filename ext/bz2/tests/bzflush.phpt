--TEST--
int bzflush(resource $bz);
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
--FILE--
<?php
$bz2String = "php-bz2-data-without-bzflush";
$bz2StringFlush = "php-bz2-data-with-bzflush";
$bz2File = "/003.txt.bz2";
$bz2ModeRead = "r";
$bz2ModeWrite = "w";

$bz2ResourceWrite = bzopen(dirname(__FILE__)."{$bz2File}", $bz2ModeWrite);
bzwrite($bz2ResourceWrite, $bz2String, strlen($bz2String));
bzclose($bz2ResourceWrite);

$bz2ResourceRead = bzopen(dirname(__FILE__)."{$bz2File}", $bz2ModeRead);
var_dump(bzread($bz2ResourceRead));
bzclose($bz2ResourceRead);

$bz2ResourceWriteFlush = bzopen(dirname(__FILE__)."{$bz2File}", $bz2ModeWrite);
bzflush($bz2ResourceWriteFlush);
bzwrite($bz2ResourceWriteFlush, $bz2StringFlush, strlen($bz2StringFlush));
bzclose($bz2ResourceWriteFlush);

$bz2ResourceReadFlush = bzopen(dirname(__FILE__)."{$bz2File}", $bz2ModeRead);
var_dump(bzread($bz2ResourceReadFlush));
bzclose($bz2ResourceReadFlush);

var_dump(bzflush());
var_dump(bzflush(""));
var_dump(bzflush(null));
var_dump(bzflush(1.2));
var_dump(bzerror($bz2ResourceWrite));
var_dump(bzerror($bz2ResourceRead));
var_dump(bzerror($bz2ResourceWriteFlush));
var_dump(bzerror($bz2ResourceReadFlush));
?>
--EXPECTF--
string(%d) "php-bz2-data-without-bzflush"
string(%d) "php-bz2-data-with-bzflush"

Warning: bzflush() expects exactly %d parameter, %d given in %s on line %d
bool(false)

Warning: bzflush() expects parameter %d to be resource, string given in %s on line %d
bool(false)

Warning: bzflush() expects parameter %d to be resource, null given in %s on line %d
bool(false)

Warning: bzflush() expects parameter %d to be resource, double given in %s on line %d
bool(false)

Warning: bzerror(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: bzerror(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: bzerror(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: bzerror(): %d is not a valid stream resource in %s on line %d
bool(false)
