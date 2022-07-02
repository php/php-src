--TEST--
Test mb_substitute_character() function : usage variation
--EXTENSIONS--
mbstring
--FILE--
<?php
echo "*** Testing mb_substitute_character(): various types in weak typing mode ***\n";

// Initialise function arguments not being substituted (if any)

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

// get a resource variable
$fp = fopen(__FILE__, "r");

// add arrays
$index_array = array (1, 2, 3);
$assoc_array = array ('one' => 1, 'two' => 2);

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
      'float 10.0e19' => 10.0e19, // Cannot be represented as int
      'float -10.0e19' => -10.0e19, // Cannot be represented as int
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
);

// loop through each element of the array for substchar

mb_internal_encoding('utf-8');
foreach($inputs as $key =>$value) {
      echo "--$key--\n";
      try {
          var_dump( mb_substitute_character($value) );
      } catch (\ValueError|\TypeError $e) {
          echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
      }
}

fclose($fp);

?>
--EXPECTF--
*** Testing mb_substitute_character(): various types in weak typing mode ***
--int 0--
bool(true)
--int 1--
bool(true)
--int 12345--
bool(true)
--int -12345--
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) is not a valid codepoint
--float 10.5--

Deprecated: Implicit conversion from float 10.5 to int loses precision in %s on line %d
bool(true)
--float -10.5--

Deprecated: Implicit conversion from float -10.5 to int loses precision in %s on line %d
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) is not a valid codepoint
--float 10.0e19--
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
--float -10.0e19--
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
--float .5--

Deprecated: Implicit conversion from float 0.5 to int loses precision in %s on line %d
bool(true)
--empty array--
TypeError: mb_substitute_character(): Argument #1 ($substitute_character) must be of type string|int|null, array given
--int indexed array--
TypeError: mb_substitute_character(): Argument #1 ($substitute_character) must be of type string|int|null, array given
--associative array--
TypeError: mb_substitute_character(): Argument #1 ($substitute_character) must be of type string|int|null, array given
--nested arrays--
TypeError: mb_substitute_character(): Argument #1 ($substitute_character) must be of type string|int|null, array given
--uppercase NULL--
int(0)
--lowercase null--
int(0)
--lowercase true--
bool(true)
--lowercase false--
bool(true)
--uppercase TRUE--
bool(true)
--uppercase FALSE--
bool(true)
--empty string DQ--
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
--empty string SQ--
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
--string DQ--
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
--string SQ--
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
--mixed case string--
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
--heredoc--
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
--instance of classWithToString--
ValueError: mb_substitute_character(): Argument #1 ($substitute_character) must be "none", "long", "entity" or a valid codepoint
--instance of classWithoutToString--
TypeError: mb_substitute_character(): Argument #1 ($substitute_character) must be of type string|int|null, classWithoutToString given
--undefined var--
int(0)
--unset var--
int(0)
