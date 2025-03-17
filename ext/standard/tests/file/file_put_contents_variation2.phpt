--TEST--
Test file_put_contents() function : usage variation - different data types to write
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing file_put_contents() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum) {
    if (error_reporting() & $err_no) {
        // report non-silenced errors
        echo "Error: $err_no - $err_msg\n";
    }
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)

$filename = __DIR__ . '/file_put_contents_variation2.tmp';

// define some classes
class classWithToString
{
    public function __toString() {
        return "Class A object";
    }
}

//array of values to iterate over
$inputs = array(
      // int data
      'int 0' => 0,
      'int 1' => 1,
      'int 12345' => 12345,
      'int -12345' => -2345,

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
      'float 12.3456789000e10' => 12.3456789000e10,
      'float -12.3456789000e10' => -12.3456789000e10,
      'float .5' => .5,


      // null data
      'null' => null,

      // boolean data
      'true' => true,
      'false' =>false,
      // empty data
      'empty string' => '',

      // object data
      'stringable' => new classWithToString(),
);

// loop through each element of the array for str

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      file_put_contents($filename, $value);
      readfile($filename);
};

?>
--CLEAN--
<?php
$filename = __DIR__ . '/file_put_contents_variation2.tmp';
unlink($filename);
?>
--EXPECT--
*** Testing file_put_contents() : usage variation ***

--int 0--
0
--int 1--
1
--int 12345--
12345
--int -12345--
-2345
--float 10.5--
10.5
--float -10.5--
-10.5
--float 12.3456789000e10--
123456789000
--float -12.3456789000e10--
-123456789000
--float .5--
0.5
--null--
Error: 8192 - file_put_contents(): Passing null to parameter #2 ($data) of type string|array|resource is deprecated

--true--
1
--false--

--empty string--

--stringable--
Class A object
