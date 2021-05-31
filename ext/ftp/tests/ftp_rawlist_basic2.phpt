--TEST--
Testing ftp_rawlist returns false on server error
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
ftp_login($ftp, 'user', 'pass');
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_rawlist($ftp, 'no_exists/'));
?>
--EXPECT--
bool(false)
