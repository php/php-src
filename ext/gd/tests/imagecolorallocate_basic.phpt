--TEST--
Test imagecolorallocate() function : basic functionality
--EXTENSIONS--
gd
--SKIPIF--
<?php
if(!function_exists('imagecreatetruecolor')) {
    die('skip imagecreatetruecolor function is not available');
}
?>
--FILE--
<?php
echo "*** Testing imagecolorallocate() : basic functionality ***\n";

$im = imagecreatetruecolor(200, 200);
// Calling imagecolorallocate() with all possible arguments
var_dump( imagecolorallocate($im, 255, 0, 0) );
var_dump( imagecolorallocate($im, 0, 255, 0) );
var_dump( imagecolorallocate($im, 0, 0, 255) );
var_dump( imagecolorallocate($im, 255, 255, 255) );
?>
--EXPECT--
*** Testing imagecolorallocate() : basic functionality ***
int(16711680)
int(65280)
int(255)
int(16777215)
