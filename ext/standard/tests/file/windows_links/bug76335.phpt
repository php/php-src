--TEST--
Bug #76335 "link(): Bad file descriptor" with non-ASCII path
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
    die('skip windows only test');
}
?>
--FILE--
<?php

$d0 = dirname(__FILE__) . DIRECTORY_SEPARATOR . "á";
$d1 = dirname(__FILE__) . DIRECTORY_SEPARATOR . "a";

$fn = dirname(__FILE__) . DIRECTORY_SEPARATOR . "file";

$l0 = $d0 . DIRECTORY_SEPARATOR . "b";
$l1 = $d1 . DIRECTORY_SEPARATOR . "b";

mkdir($d0);
mkdir($d1);

file_put_contents($fn, "");

chdir($d0);
var_dump(link($d0 . DIRECTORY_SEPARATOR . ".." . DIRECTORY_SEPARATOR . "file", $l0));

chdir($d1);
var_dump(link($d1 . DIRECTORY_SEPARATOR . ".." . DIRECTORY_SEPARATOR . "file", $l1));

?>
--CLEAN--
<?php

$d0 = dirname(__FILE__) . DIRECTORY_SEPARATOR . "á";
$d1 = dirname(__FILE__) . DIRECTORY_SEPARATOR . "a";

$fn = dirname(__FILE__) . DIRECTORY_SEPARATOR . "file";

$l0 = $d0 . DIRECTORY_SEPARATOR . "b";
$l1 = $d1 . DIRECTORY_SEPARATOR . "b";

unlink($l0);
unlink($l1);
unlink($fn);
rmdir($d0);
rmdir($d1);
?>
--EXPECT--
bool(true)
bool(true)
