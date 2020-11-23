--TEST--
Testing that imageloadfont() breaks on invalid file passed as first argument
--CREDITS--
Austin Drouare <austin.drouare [at] gmail [dot] com> #testfest #tek11
--SKIPIF--
<?php
    if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
var_dump( imageloadfont('\src\invalidfile.font') );
?>
--EXPECTF--
Warning: imageloadfont(\src\invalidfile.font): Failed to open stream: No such file or directory in %s on line %d
bool(false)
