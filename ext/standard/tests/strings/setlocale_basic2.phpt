--TEST--
Test setlocale() function : basic functionality - set locale using an array
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
                if locale  functinality is not implemented in this platform. 
 * Source code: ext/standard/string.c
*/

/* Test the setlocale() when an array is provided as input containing list of locales */

/* Prototype  : array list_system_locales( void )
 * Description: To get the currently installed locle in this platform 
 * Arguments  : Nil
 * Returns    : set of locale as array
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

/* Collect existing system locales and prepare a list of locales that can be used as 
   input to setlocale() */

echo "*** Testing setlocale() with an array containing list of locales ***\n";

//set of locales to be used
$common_locales = array(
  "english_US"=> "en_US.utf8", 
  "english_AU" => "en_AU.utf8", 
  "korean_KR" => "ko_KR.utf8", 
  "Chinese_zh" => "zh_CN.utf8",
  "germen_DE" => "de_DE.utf8",
  "spanish_es" => "es_EC.utf8",
  "french_FR" => "fr_FR.utf8",
  "japanees_JP" => "ja_JP.utf8",
  "greek_GR" => "el_GR.utf8",
  "dutch_NL" => "nl_NL.utf8"
);

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

// prepare the list of locales based on list of locales found in the system
// and those known to this script ( as stored $common_locales) which can be
// given as input to setlocale(), later verify the new locale setting by
// checking the currency setting of the system(use localconv())
$list_of_locales = array();
foreach($common_locales as $value) {
  if( in_array($value, $all_system_locales) ) {
    $list_of_locales[] = $value;
  }
}

// Now $list_of_locales array contains the locales that can be passed to 
// setlocale() function.
echo "-- Testing setlocale() : 'category' argument as LC_ALL & 'locale' argument as an array --\n";
if ( count($list_of_locales) > 0 ) {
  // set locale to $list_of_locales
  $new_locale = setlocale(LC_ALL, $list_of_locales);

  // dump the current locale
  var_dump($new_locale);

  // check that new locale setting is effective
  // use localeconv() to get the details of currently set locale
  $locale_info = localeconv();
  $new_currency = trim($locale_info['int_curr_symbol']);

  echo "Checking currency settings in the new locale, expected: ".$currency_symbol[$new_locale].", Found: ".$new_currency."\n";
  echo "Test ";

  if(trim($currency_symbol[$new_locale]) == $new_currency){
    echo "PASSED.\n";
  } else { 
    echo "FAILED.\n";
  }
} else {
  echo "Test FAILED.\n";
}

echo "Done\n";
?>
--EXPECTF--
*** Testing setlocale() with an array containing list of locales ***
-- Testing setlocale() : 'category' argument as LC_ALL & 'locale' argument as an array --
string(%d) "%s"
Checking currency settings in the new locale, expected: %s, Found: %s
Test PASSED.
Done
