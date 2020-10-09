--TEST--
Test mb_stripos() function : basic functionality
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stripos') or die("skip mb_stripos() is not available in this build");
?>
--FILE--
<?php
/*
 * Test basic functionality of mb_stripos with ASCII and multibyte characters
 */

echo "*** Testing mb_stripos() : basic functionality***\n";

mb_internal_encoding('UTF-8');

//ascii strings
$ascii_haystacks = array(
   'abc defabc   def',
   'ABC DEFABC   DEF',
   'Abc dEFaBC   Def',
);

$ascii_needles = array(
   // 4 good ones
   'DE',
   'de',
   'De',
   'dE',
);

//greek strings in UTF-8
$greek_lower = base64_decode('zrrOu868zr3Ovs6/z4DPgSDOus67zrzOvc6+zr/PgA==');
$greek_upper = base64_decode('zprOm86czp3Ons6fzqDOoSDOms6bzpzOnc6ezp/OoA==');
$greek_mixed = base64_decode('zrrOu868zr3Ovs6fzqDOoSDOus67zpzOnc6+zr/OoA==');
$greek_haystacks = array($greek_lower, $greek_upper, $greek_mixed);

$greek_nlower = base64_decode('zrzOvc6+zr8=');
$greek_nupper = base64_decode('zpzOnc6ezp8=');
$greek_nmixed1 = base64_decode('zpzOnc6+zr8=');
$greek_nmixed2 = base64_decode('zrzOvc6+zp8=');

$greek_needles = array(
   // 4 good ones
   $greek_nlower, $greek_nupper, $greek_nmixed1, $greek_nmixed2,
);

// try the basic options
echo "\n -- ASCII Strings --\n";
foreach ($ascii_needles as $needle) {
   foreach ($ascii_haystacks as $haystack) {
      var_dump(mb_stripos($haystack, $needle));
      var_dump(mb_stripos($haystack, $needle, 6));
   }
}

echo "\n -- Greek Strings --\n";
foreach ($greek_needles as $needle) {
   foreach ($greek_haystacks as $haystack) {
      var_dump(mb_stripos($haystack, $needle));
      var_dump(mb_stripos($haystack, $needle, 4));
   }
}

echo "Done";
?>
--EXPECT--
*** Testing mb_stripos() : basic functionality***

 -- ASCII Strings --
int(4)
int(13)
int(4)
int(13)
int(4)
int(13)
int(4)
int(13)
int(4)
int(13)
int(4)
int(13)
int(4)
int(13)
int(4)
int(13)
int(4)
int(13)
int(4)
int(13)
int(4)
int(13)
int(4)
int(13)

 -- Greek Strings --
int(2)
int(11)
int(2)
int(11)
int(2)
int(11)
int(2)
int(11)
int(2)
int(11)
int(2)
int(11)
int(2)
int(11)
int(2)
int(11)
int(2)
int(11)
int(2)
int(11)
int(2)
int(11)
int(2)
int(11)
Done
