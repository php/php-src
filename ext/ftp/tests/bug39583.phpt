--TEST--
Bug #39583 (FTP always transfers in binary mode)
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
$bug39583=1;
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));

$source_file = __FILE__;
$destination_file = basename(__FILE__);

// upload the file
$upload = ftp_put($ftp, $destination_file, $source_file, FTP_ASCII);

// check upload status
if (!$upload) {
       echo "FTP upload has failed!";
   } else {
       echo "Uploaded $source_file as $destination_file";
   }

// close the FTP stream
ftp_close($ftp);
?>
--EXPECTF--
bool(true)
Uploaded %sbug39583.php as bug39583.php
