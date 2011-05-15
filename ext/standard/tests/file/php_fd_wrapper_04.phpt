--TEST--
php://fd wrapper: invalid file descriptor
--SKIPIF--
<?php include('skipif.inc'); 
if(substr(PHP_OS, 0, 3) == "WIN")
	die("skip Not for Windows");

//we'd need a release and a test variation for windows, because in debug builds we get this message:
//Warning: Invalid parameter detected in CRT function '_dup' (f:\dd\vctools\crt_bld\self_x86\crt\src\dup.c:52)
//I greped the CRT sources and found no function capable of validating a file descriptor

--FILE--
<?php
fopen("php://fd/12", "w");

echo "\nDone.\n";
--EXPECTF--
Warning: fopen(php://fd/12): failed to open stream: Error duping file descriptor 12; possibly it doesn't exist: [9]: %s in %s on line %d

Done.
