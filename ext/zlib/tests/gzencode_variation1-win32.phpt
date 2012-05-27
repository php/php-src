--TEST--
Test gzencode() function : variation
--SKIPIF--
<?php 

if( substr(PHP_OS, 0, 3) != "WIN" ) {
  die("skip only for Windows");
}

if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}	 
?> 
--FILE--
<?php
/* Prototype  : string gzencode  ( string $data  [, int $level  [, int $encoding_mode  ]] )
 * Description: Gzip-compress a string 
 * Source code: ext/zlib/zlib.c
 * Alias to functions: 
 */

if(!function_exists("gzdecode")) {
	function gzdecode($data) 
	{ 
	   return gzinflate(substr($data,10,-8)); 
	}
}


include(dirname(__FILE__) . '/data.inc');

echo "*** Testing gzencode() : variation ***\n";

echo "\n-- Testing multiple compression --\n";
$output = gzencode(gzencode($data));

$back = gzdecode(gzdecode($output));
var_dump($data === $back);
?>
===Done===
--EXPECT--
*** Testing gzencode() : variation ***

-- Testing multiple compression --
bool(true)
===Done===
