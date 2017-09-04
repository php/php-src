--TEST--
Edge-cases in constant conditional jump elimination
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip for machines with 64-bit longs"); ?>
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$webserver = "Apache";
$cpuArc = "x86_64";
$archName = (strstr($cpuArc, "64") || PHP_INT_SIZE === 8) ? "64" : "32";
$info = array('os' => PHP_OS,
              'phpversion' => phpversion(),
              'arch' => $archName,
              'webserver' =>$webserver);
header('Content-Type: application/json');
echo json_encode($info) . "\n";
--EXPECT--
{"os":"Linux","phpversion":"7.2.0-dev","arch":"64","webserver":"Apache"}
