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
  [u"sysname"]=>
  string(%d) "%s"
  [u"nodename"]=>
  string(%d) "%s"
  [u"release"]=>
  string(%d) "%s"
  [u"version"]=>
  string(%d) "%s"
  [u"machine"]=>
  string(%d) "%s"
}
===DONE===
