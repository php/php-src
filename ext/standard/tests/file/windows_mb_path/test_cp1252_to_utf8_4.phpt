--TEST--
Test mkdir/rmdir cp1252 to UTF-8 path 
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_no_required_exts();
skip_if_not_win();

?>
--FILE--
<?php
/*
#vim: set fileencoding=cp1252
#vim: set encoding=cp1252
*/

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$path = dirname(__FILE__) . DIRECTORY_SEPARATOR . "data" . DIRECTORY_SEPARATOR . "Vol��ao3"; // cp1252 string
$pathw = iconv('cp1252', 'utf-8', $path);

$subpath = $path . DIRECTORY_SEPARATOR . "Vol��ao4";
$subpathw = iconv('cp1252', 'utf-8', $subpath);

/* The mb dirname exists*/
var_dump(file_exists($pathw));

var_dump(mkdir($subpathw));
var_dump(file_exists($subpathw));

get_basename_with_cp($subpathw, 65001);

var_dump(rmdir($subpathw));

?>
===DONE===
--EXPECTF--	
bool(true)
bool(true)
bool(true)
Active code page: 65001
getting basename of %s\Voláçao3\Voláçao4
string(10) "Voláçao4"
bool(true)
string(%d) "%s\Voláçao3\Voláçao4"
Active code page: %d
bool(true)
===DONE===
