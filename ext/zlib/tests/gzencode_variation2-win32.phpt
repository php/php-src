--TEST--
Test gzencode() function : variation - verify header contents with all encoding modes
--SKIPIF--
<?php 

if( substr(PHP_OS, 0, 3) != "WIN" ) {
  die("skip.. only for Windows");
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

echo "*** Testing gzencode() : variation ***\n";

$data = "A small string to encode\n";

echo "\n-- Testing with each encoding_mode  --\n";
var_dump(bin2hex(gzencode($data, -1)));
var_dump(bin2hex(gzencode($data, -1, FORCE_GZIP)));  
var_dump(bin2hex(gzencode($data, -1, FORCE_DEFLATE)));

?>
===DONE===
--EXPECTF--
*** Testing gzencode() : variation ***

-- Testing with each encoding_mode  --
string(90) "1f8b080000000000000b735428ce4dccc951282e29cacc4b5728c95748cd4bce4f49e50200d7739de519000000"
string(90) "1f8b080000000000000b735428ce4dccc951282e29cacc4b5728c95748cd4bce4f49e50200d7739de519000000"
string(66) "789c735428ce4dccc951282e29cacc4b5728c95748cd4bce4f49e50200735808cd"
===DONE===
