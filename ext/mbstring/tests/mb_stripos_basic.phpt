--TEST--
Test mb_stripos() function : basic functionality
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stripos') or die("skip mb_stripos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_stripos(string haystack, string needle [, int offset [, string encoding]])
 * Description: Finds position of first occurrence of a string within another, case insensitive 
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions: 
 */

/*
 * Test basic functionality of mb_stripos with ASCII and multibyte characters
 */

echo "*** Testing mb_stripos() : basic functionality***\n";

mb_internal_encoding('UTF-8');

//ascii strings
$ascii_haystacks = array(
   b'abc defabc   def',
   b'ABC DEFABC   DEF',
   b'Abc dEFaBC   Def',
);

$ascii_needles = array(
   // 4 good ones
   b'DE',
   b'de',
   b'De',
   b'dE',
   
   //flag a swap between good and bad
   '!', 
   
   // 4 bad ones
   b'df',
   b'Df',
   b'dF', 
   b'DF'
);

//greek strings in UTF-8
$greek_lower = base64_decode('zrHOss6zzrTOtc62zrfOuM65zrrOu868zr3Ovs6/z4DPgc+Dz4TPhc+Gz4fPiM+J');
$greek_upper = base64_decode('zpHOks6TzpTOlc6WzpfOmM6ZzprOm86czp3Ons6fzqDOoc6jzqTOpc6mzqfOqM6p');
$greek_mixed = base64_decode('zrHOss6TzpTOlc6WzpfOmM65zrrOu868zr3Ovs6fzqDOoc6jzqTOpc+Gz4fPiM+J');
$greek_haystacks = array($greek_lower, $greek_upper, $greek_mixed);

$greek_nlower = base64_decode('zrzOvc6+zr8=');
$greek_nupper = base64_decode('zpzOnc6ezp8=');
$greek_nmixed1 = base64_decode('zpzOnc6+zr8=');
$greek_nmixed2 = base64_decode('zrzOvc6+zp8=');

$greek_blower = base64_decode('zpzOns6f');
$greek_bupper = base64_decode('zrzOvs6/');
$greek_bmixed1 = base64_decode('zpzOvs6/');
$greek_bmixed2 = base64_decode('zrzOvs6f');
$greek_needles = array(
   // 4 good ones
   $greek_nlower, $greek_nupper, $greek_nmixed1, $greek_nmixed2,
   
   '!', // used to flag a swap between good and bad
   
   // 4 bad ones
   $greek_blower, $greek_bupper, $greek_bmixed1, $greek_bmixed2,   
);

// try the basic options
echo "\n -- ASCII Strings, needle should be found --\n";
foreach ($ascii_needles as $needle) {
   if ($needle == '!') {
      echo "\n -- ASCII Strings, needle should not be found --\n";
   }
   else {
      foreach ($ascii_haystacks as $haystack) {
         var_dump(mb_stripos($haystack, $needle));
      }
   }   
}

echo "\n -- Greek Strings, needle should be found --\n";
foreach ($greek_needles as $needle) {
   if ($needle == '!') {
      echo "\n -- ASCII Strings, needle should not be found --\n";
   }
   else {
      foreach ($greek_haystacks as $haystack) {
         var_dump(mb_stripos($haystack, $needle));
      }
   }   
}

echo "Done";
?>
--EXPECTF--
*** Testing mb_stripos() : basic functionality***

 -- ASCII Strings, needle should be found --
int(4)
int(4)
int(4)
int(4)
int(4)
int(4)
int(4)
int(4)
int(4)
int(4)
int(4)
int(4)

 -- ASCII Strings, needle should not be found --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

 -- Greek Strings, needle should be found --
int(11)
int(11)
int(11)
int(11)
int(11)
int(11)
int(11)
int(11)
int(11)
int(11)
int(11)
int(11)

 -- ASCII Strings, needle should not be found --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Done
