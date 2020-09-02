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
!! [ValueError] imagecolorallocate(): Argument #2 ($red) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #2 ($red) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #3 ($green) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #3 ($green) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #4 ($blue) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #4 ($blue) must be between 0 and 255 (inclusive)

--Octal 0400--
!! [ValueError] imagecolorallocate(): Argument #2 ($red) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #2 ($red) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #3 ($green) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #3 ($green) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #4 ($blue) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #4 ($blue) must be between 0 and 255 (inclusive)

--Hexa-decimal 0x100--
!! [ValueError] imagecolorallocate(): Argument #2 ($red) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #2 ($red) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #3 ($green) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #3 ($green) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #4 ($blue) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #4 ($blue) must be between 0 and 255 (inclusive)
