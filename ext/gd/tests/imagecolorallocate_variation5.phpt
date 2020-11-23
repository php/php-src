--TEST--
Test imagecolorallocate() function : usage variations  - passing octal and hexa-decimal values
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
require __DIR__ . '/func.inc';

echo "*** Testing imagecolorallocate() : usage variations ***\n";

$im = imagecreatetruecolor(200, 200);
$red = 10;
$green = 10;
$blue = 10;

$values = array(
      // octal integer data
      "Octal 000" => 000,
      "Octal 012" => 012,
      "Octal -012" => -012,
      "Octal 0377" => 0377,

      // hexa-decimal integer data
      "Hexa-decimal 0x0" => 0x0,
      "Hexa-decimal 0xA" => 0xA,
      "Hexa-decimal -0xA" => -0xA,
      "Hexa-decimal 0xFF" => 0xFF,
);

// loop through each element of the array for blue
foreach($values as $key => $value) {
    echo "\n--$key--\n";

    trycatch_dump(
        fn() => imagecolorallocate($im, $value, $green, $blue),
        fn() => imagecolorallocate($im, $red, $value, $blue),
        fn() => imagecolorallocate($im, $red, $green, $value)
    );
};
?>
--EXPECT--
*** Testing imagecolorallocate() : usage variations ***

--Octal 000--
int(2570)
int(655370)
int(657920)

--Octal 012--
int(657930)
int(657930)
int(657930)

--Octal -012--
!! [ValueError] imagecolorallocate(): Argument #2 ($red) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #3 ($green) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #4 ($blue) must be between 0 and 255 (inclusive)

--Octal 0377--
int(16714250)
int(720650)
int(658175)

--Hexa-decimal 0x0--
int(2570)
int(655370)
int(657920)

--Hexa-decimal 0xA--
int(657930)
int(657930)
int(657930)

--Hexa-decimal -0xA--
!! [ValueError] imagecolorallocate(): Argument #2 ($red) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #3 ($green) must be between 0 and 255 (inclusive)
!! [ValueError] imagecolorallocate(): Argument #4 ($blue) must be between 0 and 255 (inclusive)

--Hexa-decimal 0xFF--
int(16714250)
int(720650)
int(658175)
