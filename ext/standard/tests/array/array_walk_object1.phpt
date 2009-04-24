--TEST--
Test array_walk() function : object functionality  
--FILE--
<?php
/* Prototype  : bool array_walk(array $input, string $funcname [, mixed $userdata])
 * Description: Apply a user function to every member of an array 
 * Source code: ext/standard/array.c
*/

/*
* Pasing object in place of 'input' argument to test object functionatlity
*/

echo "*** Testing array_walk() : object functionality ***\n";

/*
 * Prototype : callback(mixed $value, mixed $key, int $addvalue
 * Parameters : $value - values in given input array
 *              $key - keys in given input array
 *              $addvalue - value to be added
 * Description : Function adds the addvalue to each element of an array
*/

function callback($value, $key, $user_data)
{
  var_dump($key);
  var_dump($value);
  var_dump($user_data);
  echo "\n";
}

class MyClass
{
  private $pri_value;
  public $pub_value;
  protected $pro_value;
  public function __construct($setVal)
  {
    $this->pri_value = $setVal;
    $this->pub_value = $setVal;
    $this->pro_value = $setVal;
  }
};    

// object for 'input' argument
$input = new MyClass(10); 

var_dump( array_walk($input, "callback", 1));

echo "Done"
?>
--EXPECTF--
*** Testing array_walk() : object functionality ***
%unicode|string%(18) "%r\0%rMyClass%r\0%rpri_value"
int(10)
int(1)

%unicode|string%(9) "pub_value"
int(10)
int(1)

%unicode|string%(12) "%r\0%r*%r\0%rpro_value"
int(10)
int(1)

bool(true)
Done
