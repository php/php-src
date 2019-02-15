--TEST--
Bug #54494: mb_substr() mishandles UTF-32LE and UCS-2LE
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php

//declare(encoding = 'UTF-8');
mb_internal_encoding('UTF-8');

header('Content-Type: text/plain; charset=UTF-32LE');

$stringOr = "hällö wörld\n";

$mode = "UTF-32LE";

echo "$mode:\n";

$string = mb_convert_encoding($stringOr, $mode);
$length = mb_strlen($string, $mode);
echo "Length: ", $length, "\n";


for ($i=0; $i < $length; $i++) {
  $t = unpack("H*",mb_substr($string, $i, 1, $mode));
  echo $t[1];
}
echo "\n";


$mode = "UCS-2LE";

echo "$mode:\n";

$string = mb_convert_encoding($stringOr, $mode);
$length = mb_strlen($string, $mode);
echo "Length: ", $length, "\n";


for ($i=0; $i < $length; $i++) {
  $t = unpack("H*",mb_substr($string, $i, 1, $mode));
  echo $t[1];
}
echo "\n";
--EXPECT--
UTF-32LE:
Length: 12
68000000e40000006c0000006c000000f60000002000000077000000f6000000720000006c000000640000000a000000
UCS-2LE:
Length: 12
6800e4006c006c00f60020007700f60072006c0064000a00
