--TEST--
Test setlocale() function : basic functionality - passing multiple locales as argument
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not valid for windows');
}
?>
--FILE--
<?php
/* Prototype  : string setlocale (int $category , string $locale [,string $..] )
              : string setlocale(int $category , array $locale);
 * Description: Sets locale information.Returns the new current locale , or FALSE
                if locale functinality is not implemented in this platform.
 * Source code: ext/standard/string.c
*/

/* Test the setlocale() when multiple locales are provided as argument */

/* Prototype  : array list_system_locales( void )
   Description: To get the currently installed locle in this platform 
   Arguments  : Nil
   Returns    : set of locale as array
*/
function list_system_locales() {
  // start the buffering of next command to internal output buffer
  ob_start();

  // run the command 'locale -a' to fetch all locales available in the system
  system('locale -a');

  // get the contents from the internal output buffer
  $all_locales = ob_get_contents();

  // fflush and end the output buffering to internal output buffer
  ob_end_clean();

  $system_locales = explode("\n", $all_locales);

  // return all the locale found in the system
  return $system_locales;
}

/* Collect existing system locales and get three locales that can be use to 
   pass as argument to setlocale() */
echo "*** Testing setlocale() by passing multiple locales as argument ***\n";


//set of currency symbol according to above list of locales
$currency_symbol = array(
  "en_US.utf8" => "USD",
  "en_AU.utf8" => "AUD",
  "ko_KR.utf8" => "KRW",
  "zh_CN.utf8" => "CNY",
  "de_DE.utf8" => "EUR",
  "es_EC.utf8" => "USD",
  "fr_FR.utf8" => "EUR",
  "ja_JP.utf8" => "JPY",
  "el_GR.utf8" => "EUR",
  "nl_NL.utf8" =>"EUR"
);

// gather all the locales installed in the system
$all_system_locales = list_system_locales();

// Now check for three locales that is present in the system and use that as argument to setlocale()
if( in_array("en_US.utf8",$all_system_locales) ||
    in_array("Ko_KR.utf8",$all_system_locales) ||
    in_array("zh_CN.utf8",$all_system_locales) ) {
  echo "-- Testing setlocale() by giving 'category' as LC_ALL & multiple locales(en_US.utf8, Ko_KR.utf8, zh_CN.utf8) --\n";

  // call setlocale()
  $new_locale = setlocale(LC_ALL, "en_US.utf8", "Ko_KR.utf8", "zh_CN.utf8");

  // dump the name of the new locale set by setlocale()
  var_dump($new_locale);

  // check that new locale setting is effective
  // use localeconv() to get the details of currently set locale
  $locale_info = localeconv();
  $new_currency = trim($locale_info['int_curr_symbol']);

  echo "Checking currency settings in the new locale, expected: ".$currency_symbol[$new_locale].", Found: ".$new_currency."\n";
  echo "Test ";
  if( trim($currency_symbol[$new_locale]) == $new_currency) {
    echo "PASSED.\n";
  } else {
    echo "FAILED.\n";
  }
}

echo "Done\n";
?>
--EXPECTF--
*** Testing setlocale() by passing multiple locales as argument ***
-- Testing setlocale() by giving 'category' as LC_ALL & multiple locales(en_US.utf8, Ko_KR.utf8, zh_CN.utf8) --
string(%d) "%s"
Checking currency settings in the new locale, expected: %s, Found: %s
Test PASSED.
Done
