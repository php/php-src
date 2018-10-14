--TEST--
Testing get_defined_vars() Function
--FILE--
<?php
/* Prototype: array get_defined_vars(void);
 * Description: Returns a  multidimentional array of all defined variables.
 */

/* Various variables definitions used for testing of the function */

$number = 22.33; //number
$string = "sample string"; //string
$array1 = array(1, 1, 2, 3, 5, 8); //simple array
$assoc_array = array( 'a'=>97, 'c'=>99, 'A'=>65, 'C'=>67, 1=>"string1" ); //associative array
$boolean = TRUE; //boolean

/* Checking for Class and Objects */
class sample {
var $number = 233;
var $string = "string2";
public function func() {
$local_var = 2;
var_dump( get_defined_vars() );
}
}
$sample_obj = new sample; //object declaration

function func() {
$string33 = 22;
var_dump( get_defined_vars() );
}

$arr = get_defined_vars();

/* Displaying various variable through the array captured by the get_defined_vars function call */
echo "\n*** Displaying various variables through the array captured by the get_defined_vars function call ***\n";
var_dump( $arr["argc"] );
var_dump( $arr["number"] );
var_dump( $arr["string"] );
var_dump( $arr["array1"] );
var_dump( $arr["assoc_array"] );
var_dump( $arr["boolean"] );
var_dump( $arr["sample_obj"] );


echo "\n*** Checking for output when get_defined_vars called in local function ***\n";
func();


echo "\n*** Checking for output when get_defined_vars called in function of a class ***\n";
$sample_obj->func();

echo "\n*** Checking for output when get_defined_vars called in nested functions ***\n";
function func1(){
$func1_var = 2;
var_dump( get_defined_vars() );
function func2(){
$func2_var = 3;
var_dump( get_defined_vars() );
}
func2();
}
func1();

echo "\nDone";
?>
--EXPECTF--
*** Displaying various variables through the array captured by the get_defined_vars function call ***
int(1)
float(22.33)
string(13) "sample string"
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(5)
  [5]=>
  int(8)
}
array(5) {
  ["a"]=>
  int(97)
  ["c"]=>
  int(99)
  ["A"]=>
  int(65)
  ["C"]=>
  int(67)
  [1]=>
  string(7) "string1"
}
bool(true)
object(sample)#1 (2) {
  ["number"]=>
  int(233)
  ["string"]=>
  string(7) "string2"
}

*** Checking for output when get_defined_vars called in local function ***
array(1) {
  ["string33"]=>
  int(22)
}

*** Checking for output when get_defined_vars called in function of a class ***
array(1) {
  ["local_var"]=>
  int(2)
}

*** Checking for output when get_defined_vars called in nested functions ***
array(1) {
  ["func1_var"]=>
  int(2)
}
array(1) {
  ["func2_var"]=>
  int(3)
}

Done
