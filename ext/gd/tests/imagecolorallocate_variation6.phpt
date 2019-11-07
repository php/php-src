--TEST--
Test imagecolorallocate() function : usage variations  - passing RED, GREEN, BLUE values more than 255
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
require  __DIR__ . '/func.inc';

/* Prototype  : int imagecolorallocate(resource im, int red, int green, int blue)
 * Description:  Allocate a color for an image
 * Source code: ext/gd/gd.c
 */
echo "*** Testing imagecolorallocate() : usage variations ***\n";

$values = array(
      //Decimal integera data
      "Decimal 256" => 256,

      // octal integer data
      "Octal 0400" => 0400,

      // hexa-decimal integer data
      "Hexa-decimal 0x100" => 0x100
);

// loop through each element of the array for blue
foreach($values as $key => $value) {
      echo "\n--$key--\n";
      //Need to be created every time to get expected return value
      $im_palette = imagecreate(200, 200);
      $im_true_color = imagecreatetruecolor(200, 200);

      trycatch_dump(
          fn() => imagecolorallocate($im_palette, $value, 0, 0),
          fn() => imagecolorallocate($im_true_color, $value, 0, 0),
          fn() => imagecolorallocate($im_palette, 0, $value, 0),
          fn() => imagecolorallocate($im_true_color, 0, $value, 0),
          fn() => imagecolorallocate($im_palette, 0, 0, $value),
          fn() => imagecolorallocate($im_true_color, 0, 0, $value)
      );
};
?>
--EXPECT--
*** Testing imagecolorallocate() : usage variations ***

--Decimal 256--
!! [ValueError] Red component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Red component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Green component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Green component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Blue component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Blue component is out of range, must be between 0 and 255 (inclusive)

--Octal 0400--
!! [ValueError] Red component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Red component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Green component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Green component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Blue component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Blue component is out of range, must be between 0 and 255 (inclusive)

--Hexa-decimal 0x100--
!! [ValueError] Red component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Red component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Green component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Green component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Blue component is out of range, must be between 0 and 255 (inclusive)
!! [ValueError] Blue component is out of range, must be between 0 and 255 (inclusive)
