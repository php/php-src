--TEST--
Test setlocale() function : usage variations - Setting all available locales in the platform
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not valid for windows');
}
exec("locale -a", $output, $exit_code);
if ($exit_code !== 0) {
    die("skip locale -a not available");
}
?>
--FILE--
<?php
/* Prototype  : string setlocale (int $category , string $locale [,string $..] )
              : string setlocale(int $category , array $locale);
 * Description: Sets locale information.Returns the new current locale , or FALSE
                if locale functionality is not implemented in this platform.
 * Source code: ext/standard/string.c
*/

/* setlocale() to set all available locales in the system and check the success count */
echo "*** Testing setlocale() : usage variations ***\n";

function good_locale($locale) {
    /**
    * Note: no_NO is a bogus locale and should not be used, see https://bugzilla.redhat.com/971416
    **/
	return $locale !== 'tt_RU@iqtelif.UTF-8' && $locale !== 'no_NO.ISO-8859-1';
}

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

  // return all the locale found in the system, except for broken one
  return array_filter($system_locales, 'good_locale');
}

// gather all the locales installed in the system
$all_system_locales = list_system_locales();

//try different locale names
$failure_locale = array();
$success_count = 0;

echo "-- Test setlocale() with all available locale in the system --\n";
// gather all locales installed in the system(stored $all_system_locales),
// try n set each locale using setlocale() and keep track failures, if any
foreach($all_system_locales as $value){
  //set locale to $value, if success, count increments
  if(setlocale(LC_ALL,$value )){
   $success_count++;
  }
  else{
   //failure values are put in to an array $failure_locale
   $failure_locale[] = $value;
  }
}

echo "No of locales found on the machine = ".count($all_system_locales)."\n";
echo "No of setlocale() success = ".$success_count."\n";
echo "Expected no of failures = 0\n";
echo "Test ";
// check if there were any failure of setlocale() function earlier, if any
// failure then dump the list of failing locales
if($success_count != count($all_system_locales)){
  echo "FAILED\n";
  echo "Names of locale() for which setlocale() failed ...\n";
  var_dump($failure_locale);
}
else{
  echo "PASSED\n";
}

echo "Done\n";
?>
--EXPECTF--
*** Testing setlocale() : usage variations ***
-- Test setlocale() with all available locale in the system --
No of locales found on the machine = %d
No of setlocale() success = %d
Expected no of failures = 0
Test PASSED
Done
