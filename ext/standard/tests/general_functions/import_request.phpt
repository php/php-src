--TEST--
import_request_variables() tests
--SKIPIF--
<?php if(PHP_VERSION_ID >= 50399){ die('skip not needed anymore without register_globals'); } ?>
--GET--
a=1&b=heh&c=3&d[]=5&GLOBALS=test&1=hm
--POST--
ap=25&bp=test&cp=blah3&dp[]=ar
--FILE--
<?php

var_dump(import_request_variables());
var_dump(import_request_variables(""));
var_dump(import_request_variables("", ""));

var_dump(import_request_variables("g", ""));
var_dump($a, $b, $c, $ap);

var_dump(import_request_variables("g", "g_"));
var_dump($g_a, $g_b, $g_c, $g_ap, $g_1);

var_dump(import_request_variables("GP", "i_"));
var_dump($i_a, $i_b, $i_c, $i_ap, $i_bp, $i_cp, $i_dp);

var_dump(import_request_variables("gGg", "r_"));
var_dump($r_a, $r_b, $r_c, $r_ap);

echo "Done\n";
?>
--EXPECTF--	
Warning: import_request_variables() expects at least 1 parameter, 0 given in %s on line %d
NULL
bool(false)

Notice: import_request_variables(): No prefix specified - possible security hazard in %s on line %d
bool(false)

Notice: import_request_variables(): No prefix specified - possible security hazard in %s on line %d

Warning: import_request_variables(): Attempted GLOBALS variable overwrite in %s on line %d

Warning: import_request_variables(): Numeric key detected - possible security hazard in %s on line %d
bool(true)

Notice: Undefined variable: ap in %s on line %d
string(1) "1"
string(3) "heh"
string(1) "3"
NULL
bool(true)

Notice: Undefined variable: g_ap in %s on line %d
string(1) "1"
string(3) "heh"
string(1) "3"
NULL
string(2) "hm"
bool(true)
string(1) "1"
string(3) "heh"
string(1) "3"
string(2) "25"
string(4) "test"
string(5) "blah3"
array(1) {
  [0]=>
  string(2) "ar"
}
bool(true)

Notice: Undefined variable: r_ap in %s on line %d
string(1) "1"
string(3) "heh"
string(1) "3"
NULL
Done
