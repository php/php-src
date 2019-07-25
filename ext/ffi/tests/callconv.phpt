--TEST--
Different calling conventions
--SKIPIF--
<?php
require_once('skipif.inc');
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip for Windows only');
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
?>
--FILE--
<?php
$header = <<<HEADER
void __cdecl cdecl_func(int arg1, double arg2);
void __stdcall stdcall_func(int arg1, double arg2);
void __fastcall fastcall_func(int arg1, double arg2);
HEADER;
$headername = __DIR__ . '/callconv.h';
$dllname = __DIR__ . "/callconv_x86.dll";

$ffi1 = FFI::cdef($header, $dllname);
$ffi1->cdecl_func(1, 2.3);
$ffi1->stdcall_func(4, 5.6);
$ffi1->fastcall_func(7, 8.9);

file_put_contents($headername, "#define FFI_LIB \"$dllname\"\n$header");

$ffi2 = FFI::load($headername);
$ffi2->cdecl_func(2, 3.4);
$ffi2->stdcall_func(5, 6.7);
$ffi2->fastcall_func(8, 9.1);
?>
--EXPECT--
cdecl: 1, 2.300000
stdcall: 4, 5.600000
fastcall: 7, 8.900000
cdecl: 2, 3.400000
stdcall: 5, 6.700000
fastcall: 8, 9.100000
--CLEAN--
<?php
unlink(__DIR__ . '/callconv.h');
?>
