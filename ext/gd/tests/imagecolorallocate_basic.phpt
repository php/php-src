--TEST--
Test imagecolorallocate() function : basic functionality
--SKIPIF--
<?php
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}
if(!function_exists('imagecreatetruecolor')) {
    die('skip imagecreatetruecolor function is not available');
}
?>
--FILE--
<?php
/* Prototype  : int imagecolorallocate(resource im, int red, int green, int blue)
 * Description: Allocate a color for an image
 * Source code: ext/gd/gd.c
 */

echo "*** Testing imagecolorallocate() : basic functionality ***\n";

$im = imagecreatetruecolor(200, 200);
// Calling imagecolorallocate() with all possible arguments
var_dump( imagecolorallocate($im, 255, 0, 0) );
var_dump( imagecolorallocate($im, 0, 255, 0) );
var_dump( imagecolorallocate($im, 0, 0, 255) );
var_dump( imagecolorallocate($im, 255, 255, 255) );
?>
===DONE===
--EXPECTF--
*** Testing imagecolorallocate() : basic functionality ***
int(16711680)
int(65280)
int(255)
int(16777215)
===DONE===
