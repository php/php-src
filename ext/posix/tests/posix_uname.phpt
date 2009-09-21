--TEST--
Test posix_uname()
--DESCRIPTION--
Gets information about the system.
Source code: ext/posix/posix.c
--CREDITS--
Falko Menge, mail at falko-menge dot de
PHP Testfest Berlin 2009-05-10
--SKIPIF--
<?php 
	if (!extension_loaded('posix')) { 
        die('SKIP - POSIX extension not available');
    }
?>
--FILE--
<?php
    $uname = posix_uname();
    unset($uname['domainname']);
    var_dump($uname);
?>
===DONE===
--EXPECTF--
array(5) {
  ["sysname"]=>
  string(%d) "%s"
  ["nodename"]=>
  string(%d) "%s"
  ["release"]=>
  string(%d) "%s"
  ["version"]=>
  string(%d) "%s"
  ["machine"]=>
  string(%d) "%s"
}
===DONE===
