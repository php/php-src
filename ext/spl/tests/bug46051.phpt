--TEST--
Bug #46051 (SplFileInfo::openFile - memory overlap)
--FILE--
<?php

$x = new splfileinfo(__FILE__);

try {
    $x->openFile(NULL, NULL, NULL);
} catch (TypeError $e) { }

var_dump($x->getPathName());
?>
--EXPECTF--
string(%d) "%sbug46051.php"
