--TEST--
Testing ftp_alloc returns true
--CREDITS--
Rodrigo Moyle <eu [at] rodrigorm [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");
ftp_login($ftp, 'user', 'pass');

var_dump(ftp_alloc($ftp, 1024, $result));
var_dump($result);
?>
--EXPECT--
bool(true)
string(20) "1024 bytes allocated"
