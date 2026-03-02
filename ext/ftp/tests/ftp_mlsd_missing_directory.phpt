--TEST--
Testing ftp_mlsd returns false on server error
--CREDITS--
Andreas Treichel <gmblar+github [at] gmail [dot] com>
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_mlsd($ftp, 'no_exists/'));
?>
--EXPECT--
bool(false)
