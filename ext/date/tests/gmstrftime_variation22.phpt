--TEST--
Test gmstrftime() function : usage variation - Checking Preferred date and time representation other than on Windows.
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) == 'WIN') {
    die("skip Test is not valid for Windows");
}
if (!setlocale(LC_TIME, "POSIX")) {
    die("skip Locale POSIX is required to run this test");
}
?>
--FILE--
<?php
echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);
setlocale(LC_TIME, "POSIX");
putenv("LC_TIME=POSIX");
date_default_timezone_set("Asia/Calcutta");

//array of values to iterate over
$inputs = array(
      'Preferred date and time representation' => "%c",
      'Preferred date representation' => "%x",
      'Preferred time representation' => "%X",
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( gmstrftime($value, $timestamp) );
};

?>
--EXPECTF--
*** Testing gmstrftime() : usage variation ***

--Preferred date and time representation--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(24) "Fri Aug  8 08:08:08 2008"

--Preferred date representation--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(8) "08/08/08"

--Preferred time representation--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(8) "08:08:08"
