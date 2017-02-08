--TEST--
ftp_mlsd() must not return false on empty directories
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

var_dump(ftp_mlsd($ftp, ''));
var_dump(ftp_mlsd($ftp, 'emptydir'));
var_dump(ftp_mlsd($ftp, 'bogusdir'));

ftp_close($ftp);
?>
--EXPECT--
bool(true)
array(3) {
  [0]=>
  string(109) "modify=20170127230002;perm=flcdmpe;type=cdir;unique=811U4340002;UNIX.group=33;UNIX.mode=0755;UNIX.owner=33; ."
  [1]=>
  string(110) "modify=20170127230002;perm=flcdmpe;type=pdir;unique=811U4340002;UNIX.group=33;UNIX.mode=0755;UNIX.owner=33; .."
  [2]=>
  string(122) "modify=20170126121225;perm=adfrw;size=4729;type=file;unique=811U4340CB9;UNIX.group=33;UNIX.mode=0644;UNIX.owner=33; foobar"
}
array(0) {
}
bool(false)
