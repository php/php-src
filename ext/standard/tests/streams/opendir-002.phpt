--TEST--
opendir() with 'ftp://' stream. 
--SKIPIF--
<?php if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available."); ?>
--FILE--
<?php

require __DIR__ . "/../../../ftp/tests/server.inc";

$path="ftp://localhost:" . $port."/";

var_dump(opendir($path));
?>
==DONE== 
--EXPECTF--
resource(%d) of type (stream)
==DONE==
