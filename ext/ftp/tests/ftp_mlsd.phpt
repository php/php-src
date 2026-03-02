--TEST--
ftp_mlsd() return parsed lines
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));

var_dump(ftp_mlsd($ftp, '.'));

ftp_close($ftp);
?>
--EXPECTF--
bool(true)

Warning: ftp_mlsd(): Missing pathname in MLSD response in %s on line %d

Warning: ftp_mlsd(): Malformed fact in MLSD response in %s on line %d

Warning: ftp_mlsd(): Malformed fact in MLSD response in %s on line %d
array(4) {
  [0]=>
  array(8) {
    ["name"]=>
    string(1) "."
    ["modify"]=>
    string(14) "20170127230002"
    ["perm"]=>
    string(7) "flcdmpe"
    ["type"]=>
    string(4) "cdir"
    ["unique"]=>
    string(11) "811U4340002"
    ["UNIX.group"]=>
    string(2) "33"
    ["UNIX.mode"]=>
    string(4) "0755"
    ["UNIX.owner"]=>
    string(2) "33"
  }
  [1]=>
  array(8) {
    ["name"]=>
    string(2) ".."
    ["modify"]=>
    string(14) "20170127230002"
    ["perm"]=>
    string(7) "flcdmpe"
    ["type"]=>
    string(4) "pdir"
    ["unique"]=>
    string(11) "811U4340002"
    ["UNIX.group"]=>
    string(2) "33"
    ["UNIX.mode"]=>
    string(4) "0755"
    ["UNIX.owner"]=>
    string(2) "33"
  }
  [2]=>
  array(9) {
    ["name"]=>
    string(6) "foobar"
    ["modify"]=>
    string(14) "20170126121225"
    ["perm"]=>
    string(5) "adfrw"
    ["size"]=>
    string(4) "4729"
    ["type"]=>
    string(4) "file"
    ["unique"]=>
    string(11) "811U4340CB9"
    ["UNIX.group"]=>
    string(2) "33"
    ["UNIX.mode"]=>
    string(4) "0644"
    ["UNIX.owner"]=>
    string(2) "33"
  }
  [3]=>
  array(3) {
    ["name"]=>
    string(9) "path;name"
    ["fact"]=>
    string(6) "val=ue"
    ["empty"]=>
    string(0) ""
  }
}
