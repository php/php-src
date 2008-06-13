--TEST--
Test gd_info() function : variation - Checking all the values in returned array
--SKIPIF--
<?php
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}
if(!function_exists('gd_info')) {
    die('skip gd_info function is not available');
}
?>
--FILE--
<?php
/* Prototype  : array gd_info()
 * Description: Retrieve information about the currently installed GD library
 * Source code: ext/gd/gd.c
 */

echo "*** Testing gd_info() : variation ***\n";

var_dump(gd_info());
?>
===DONE===
--EXPECTF--
*** Testing gd_info() : variation ***
array(%d) {
  [u"GD Version"]=>
  unicode(%d) %a
  [u"FreeType Support"]=>
  bool%a
  [u"T1Lib Support"]=>
  bool%a
  [u"GIF Read Support"]=>
  bool%a
  [u"GIF Create Support"]=>
  bool%a
  [u"JPG Support"]=>
  bool%a
  [u"PNG Support"]=>
  bool%a
  [u"WBMP Support"]=>
  bool%a
  [u"XPM Support"]=>
  bool%a
  [u"XBM Support"]=>
  bool%a
  [u"JIS-mapped Japanese Font Support"]=>
  bool%a
===DONE===  	