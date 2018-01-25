--TEST--
Test the File Modification Time as described in http://tools.ietf.org/html/rfc3659#section-3.1
--CREDITS--
Nathaniel McHugh
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php

require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));


date_default_timezone_set('UTC');

$time = ftp_mdtm($ftp, "A");
echo date("F d Y H:i:s u",$time), PHP_EOL;

$time = ftp_mdtm($ftp, "B");
echo date("F d Y H:i:s u",$time), PHP_EOL;

$time = ftp_mdtm($ftp, "C");
echo date("F d Y H:i:s u",$time), PHP_EOL;

$time = ftp_mdtm($ftp, "D");
var_dump($time);

$time = ftp_mdtm($ftp, "19990929043300 File6");
echo date("F d Y H:i:s u",$time), PHP_EOL;

$time = ftp_mdtm($ftp, "MdTm 19990929043300 file6");
var_dump($time);

?>
--EXPECTF--
bool(true)
June 15 1998 10:00:45 000000
June 15 1998 10:00:45 000000
July 05 1998 13:23:16 000000
int(-1)
October 05 1999 21:31:02 000000
int(-1)
