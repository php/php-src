--TEST--
Test gmstrftime() function : usage variation - Checking time related formats which are supported other than on Windows.
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) == 'WIN') {
    die("skip Test is not valid for Windows");
}
?>
--FILE--
<?php
echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(14, 8, 8, 8, 8, 2008);
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");

//array of values to iterate over
$inputs = array(
      'Time in a.m/p.m notation' => "%r",
      'Time in 24 hour notation' => "%R",
      'Current time %H:%M:%S format' => "%T",
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( gmstrftime($value) );
      var_dump( gmstrftime($value, $timestamp) );
};

?>
--EXPECTF--
*** Testing gmstrftime() : usage variation ***

--Time in a.m/p.m notation--
string(%d) "%d:%d:%d %s"
string(11) "02:08:08 PM"

--Time in 24 hour notation--
string(%d) "%d:%d"
string(5) "14:08"

--Current time %H:%M:%S format--
string(%d) "%d:%d:%d"
string(8) "14:08:08"
