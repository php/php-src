--TEST--
SPL: SplFileObject realpath and include_path
--FILE--
<?php

set_include_path('tests');

chdir(dirname(dirname(__FILE__))); // ext/spl


$fo = new SplFileObject('fileobject_004.phpt', 'r', true);

var_dump($fo->getPath());
var_dump($fo->getFilename());
var_dump($fo->getRealPath());
?>
==DONE==
--EXPECTF--
unicode(%d) "%sspl%stests"
unicode(19) "fileobject_004.phpt"
unicode(%d) "%sspl%stests%sfileobject_004.phpt"
==DONE==
