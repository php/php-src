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
/* Prototype  : int imagecolorallocate(resource im, int red, int green, int blue)
 * Description:  Allocate a color for an image
 * Source code: ext/gd/gd.c
 */
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
      var_dump( imagecolorallocate($im, $value, $green, $blue) );
      var_dump( imagecolorallocate($im, $red, $value, $blue) );
      var_dump( imagecolorallocate($im, $red, $green, $value) );
};
?>
===DONE===
--EXPECTF--
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
bool(false)
int(652810)
int(657910)

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
bool(false)
int(652810)
int(657910)

--Hexa-decimal 0xFF--
int(16714250)
int(720650)
int(658175)
===DONE===
