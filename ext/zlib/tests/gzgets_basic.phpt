--TEST--
Test function gzgets() by calling it with its expected arguments
--EXTENSIONS--
zlib
--FILE--
<?php

// note that gzgets is an alias to fgets. parameter checking tests will be
// the same as fgets

$f = __DIR__."/004.txt.gz";
$h = gzopen($f, 'r');
$lengths = array(10, 14, 7, 99);
foreach ($lengths as $length) {
   var_dump(gzgets( $h, $length ) );
}

while (gzeof($h) === false) {
   var_dump(gzgets($h));
}
gzclose($h);


?>
--EXPECT--
string(9) "When you'"
string(13) "re taught thr"
string(6) "ough f"
string(8) "eelings
"
string(26) "Destiny flying high above
"
string(38) "all I know is that you can realize it
"
string(18) "Destiny who cares
"
string(19) "as it turns around
"
string(39) "and I know that it descends down on me
"
