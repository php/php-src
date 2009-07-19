--TEST--
Test imagecolorallocate() function : usage variations  - passing different data types to fourth argument
--SKIPIF--
<?php
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}
if(!function_exists('imagecreatetruecolor')) {
    die('skip imagecreatetruecolor function is not available');
}
?>
--FILE--
<?php
/* Prototype  : int imagecolorallocate(resource im, int red, int green, int blue)
 * Description:  Allocate a color for an image
 * Source code: ext/gd/gd.c
 */
echo "*** Testing imagecolorallocate() : usage variations ***\n";

$im = imagecreatetruecolor(200, 200);
$red = 10;
$green = 10;

$fp = tmpfile();

//get an unset variable
$unset_var = 10;
unset ($unset_var);
// define some classes
class classWithToString
{
        public function __toString() {
                return "Class A object";
        }
}


class classWithoutToString
{
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// add arrays
$index_array = array (1, 2, 3);
$assoc_array = array ('one' => 1, 'two' => 2);

//array of values to iterate over
$values = array(

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
      'float 10.1234567e10' => 10.1234567e10,
      'float 10.7654321E-10' => 10.7654321E-10,
      'float .5' => .5,

      // array data
      'empty array' => array(),
      'int indexed array' => $index_array,
      'associative array' => $assoc_array,
	  'nested arrays' => array('foo', $index_array, $assoc_array),
      
      // null data
	  'uppercase NULL' => NULL,
      'lowercase null' => null,

      // boolean data
      'lowercase true' => true,
      'lowercase false' =>false,
      'uppercase TRUE' =>TRUE,
      'uppercase FALSE' =>FALSE,

      // empty data
      'empty string DQ' => "",
      'empty string SQ' => '',

      // string data
      'string DQ' => "string",
      'string SQ' => 'string',
      'mixed case string' => "sTrInG",
      'heredoc' => $heredoc,

      // object data
      'instance of classWithToString' => new classWithToString(),
      'instance of classWithoutToString' => new classWithoutToString(),

      // undefined data
      'undefined var' => @$undefined_var,

      // unset data
      'unset var' => @$unset_var,
      
      //resource 
      "file resource" => $fp
);
// loop through each element of the array for red
foreach($values as $key => $value) {
      echo "\n--$key--\n";
      var_dump( imagecolorallocate($im, $red, $green, $value) );
};
?>
===DONE===
--EXPECTF--
*** Testing imagecolorallocate() : usage variations ***

--float 10.5--
int(657930)

--float -10.5--
int(657910)

--float 10.1234567e10--
int(657919)

--float 10.7654321E-10--
int(657920)

--float .5--
int(657920)

--empty array--
int(657920)

--int indexed array--
int(657921)

--associative array--
int(657921)

--nested arrays--
int(657921)

--uppercase NULL--
int(657920)

--lowercase null--
int(657920)

--lowercase true--
int(657921)

--lowercase false--
int(657920)

--uppercase TRUE--
int(657921)

--uppercase FALSE--
int(657920)

--empty string DQ--
int(657920)

--empty string SQ--
int(657920)

--string DQ--
int(657920)

--string SQ--
int(657920)

--mixed case string--
int(657920)

--heredoc--
int(657920)

--instance of classWithToString--

Notice: Object of class classWithToString could not be converted to int in %s on line %d
int(657921)

--instance of classWithoutToString--

Notice: Object of class classWithoutToString could not be converted to int in %s on line %d
int(657921)

--undefined var--
int(657920)

--unset var--
int(657920)

--file resource--
int(657925)
===DONE===
