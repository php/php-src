--TEST--
Test function gzread() by calling it with its expected arguments
--EXTENSIONS--
zlib
--FILE--
<?php
// note that gzread is an alias to fread. parameter checking tests will be
// the same as fread

$f = __DIR__."/004.txt.gz";
$h = gzopen($f, 'r');
$lengths = array(10, 14, 7, 99, 2000);

foreach ($lengths as $length) {
   var_dump(gzread( $h, $length ) );
}
gzclose($h);

?>
--EXPECT--
string(10) "When you'r"
string(14) "e taught throu"
string(7) "gh feel"
string(99) "ings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns "
string(46) "around
and I know that it descends down on me
"
