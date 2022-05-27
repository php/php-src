--TEST--
SPL: SplFileObject realpath and include_path
--FILE--
<?php

set_include_path('tests');

chdir(dirname(__DIR__)); // ext/spl


$fo = new SplFileObject('fileobject_004.phpt', 'r', true);

var_dump($fo->getPath());
var_dump($fo->getFilename());
var_dump($fo->getRealPath());
?>
--EXPECTF--
string(%d) "%sspl%etests%eSplFileObject"
string(19) "fileobject_004.phpt"
string(%d) "%sspl%etests%eSplFileObject%efileobject_004.phpt"
