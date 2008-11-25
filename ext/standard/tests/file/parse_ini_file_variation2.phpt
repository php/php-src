--TEST--
Test parse_ini_file() function : variation: handling different boolean values
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : array parse_ini_file(string filename [, bool process_sections])
 * Description: Parse configuration file 
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: 
 */

echo "*** Testing parse_ini_file() : variation ***\n";
$output_file = __FILE__.".ini";
$iniFile = <<<FILE
[section1]
value1=on
value2=off
[section2]
value1=true
value2=false
[section3]
value1=yes
value2=no
[section4]
value1=null
value2=

[section5]
value1="on"
value2="off"
[section6]
value1="true"
value2="false"
[section7]
value1="yes"
value2="no"
[section8]
value1="null"
value2=""

FILE;

file_put_contents($output_file, $iniFile);

$a = parse_ini_file($output_file, true);
var_dump($a);
unlink($output_file);
?>
===DONE===
--EXPECT--
*** Testing parse_ini_file() : variation ***
array(8) {
  ["section1"]=>
  array(2) {
    ["value1"]=>
    string(1) "1"
    ["value2"]=>
    string(0) ""
  }
  ["section2"]=>
  array(2) {
    ["value1"]=>
    string(1) "1"
    ["value2"]=>
    string(0) ""
  }
  ["section3"]=>
  array(2) {
    ["value1"]=>
    string(1) "1"
    ["value2"]=>
    string(0) ""
  }
  ["section4"]=>
  array(2) {
    ["value1"]=>
    string(0) ""
    ["value2"]=>
    string(0) ""
  }
  ["section5"]=>
  array(2) {
    ["value1"]=>
    string(2) "on"
    ["value2"]=>
    string(3) "off"
  }
  ["section6"]=>
  array(2) {
    ["value1"]=>
    string(4) "true"
    ["value2"]=>
    string(5) "false"
  }
  ["section7"]=>
  array(2) {
    ["value1"]=>
    string(3) "yes"
    ["value2"]=>
    string(2) "no"
  }
  ["section8"]=>
  array(2) {
    ["value1"]=>
    string(4) "null"
    ["value2"]=>
    string(0) ""
  }
}
===DONE===
