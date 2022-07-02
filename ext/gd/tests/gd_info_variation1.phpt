--TEST--
Test gd_info() function : variation - Checking all the values in returned array
--EXTENSIONS--
gd
--SKIPIF--
<?php
if(!function_exists('gd_info')) {
    die('skip gd_info function is not available');
}
?>
--FILE--
<?php
echo "*** Testing gd_info() : variation ***\n";

var_dump(gd_info());
?>
--EXPECTF--
*** Testing gd_info() : variation ***
array(%d) {
  ["GD Version"]=>
  string(%d) %s
  ["FreeType Support"]=>
  bool(%a
  ["GIF Read Support"]=>
  bool(%s)
  ["GIF Create Support"]=>
  bool(%s)
  ["JPEG Support"]=>
  bool(%s)
  ["PNG Support"]=>
  bool(%s)
  ["WBMP Support"]=>
  bool(%s)
  ["XPM Support"]=>
  bool(%s)
  ["XBM Support"]=>
  bool(%s)
  ["WebP Support"]=>
  bool(%s)
  ["BMP Support"]=>
  bool%a
  ["JIS-mapped Japanese Font Support"]=>
  bool(%s)
}
