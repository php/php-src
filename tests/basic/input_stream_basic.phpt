--TEST--
input_stream_basic reading from php://input
--PUT--
Content-Type: application/octet-stream

12345
--FILE--
<?php
$tmp_dir = ini_get('upload_tmp_dir') ? ini_get('upload_tmp_dir') : sys_get_temp_dir();
$p = "$tmp_dir/php*";
var_dump(count(glob($p))); // there should be no tmp files

$h = fopen("php://input/maxmemory:2",'r');
var_dump(fread($h,1));     // "1"
var_dump(count(glob($p))); // there should still be no tmp file

var_dump(fread($h,1));     // "2"
var_dump(count(glob($p))); // there should still be no tmp file

var_dump(fread($h,1));     // "3"
var_dump(count(glob($p))); // there should be a tmp file

var_dump(fread($h,1));     // "4"
var_dump(count(glob($p))); // there should be a tmp file

var_dump(fread($h,1));     // "5"
var_dump(count(glob($p))); // there should be a tmp file
fclose($h);
var_dump(count(glob($p))); // there should still be a tmp file
var_dump(file_get_contents("php://input"));
--EXPECTF--
int(0)
string(%d) "1"
int(0)
string(%d) "2"
int(0)
string(%d) "3"
int(1)
string(%d) "4"
int(1)
string(%d) "5"
int(1)
int(1)
string(%d) "12345"
