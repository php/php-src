--TEST--
Edge-cases in constant conditional jump elimination
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip for machines with 64-bit longs"); ?>
<?php if (!extension_loaded("json")) print "skip"; ?>
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$webserver = "Apache";
$cpuArc = "x86_64";
$archName = (strstr($cpuArc, "64") || PHP_INT_SIZE === 8) ? "64" : "32";
$info = array('arch' => $archName,
              'webserver' =>$webserver);
header('Content-Type: application/json');
echo json_encode($info) . "\n";
--EXPECT--
{"arch":"64","webserver":"Apache"}
