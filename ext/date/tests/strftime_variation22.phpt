--TEST--
Test strftime() function : usage variation - Checking Preferred date and time representation other than on Windows.
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) == 'WIN') {
    die("skip Test is not valid for Windows");
}
if(!setlocale(LC_ALL, "POSIX")) {
    die("skip Locale POSIX is  needed by test and is not available");
}
?>
--FILE--
<?php
echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
setlocale(LC_ALL, "POSIX");
putenv("LC_ALL=POSIX");
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

//array of values to iterate over
$inputs = array(
      'Preferred date and time representation' => "%c",
      'Preferred date representation' => "%x",
      'Preferred time representation' => "%X",
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( strftime($value, $timestamp) );
}

?>
--EXPECTF--
*** Testing strftime() : usage variation ***

--Preferred date and time representation--

Deprecated: Function strftime() is deprecated in %s on line %d
string(24) "Fri Aug  8 08:08:08 2008"

--Preferred date representation--

Deprecated: Function strftime() is deprecated in %s on line %d
string(8) "08/08/08"

--Preferred time representation--

Deprecated: Function strftime() is deprecated in %s on line %d
string(8) "08:08:08"
