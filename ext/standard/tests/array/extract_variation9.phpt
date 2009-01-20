--TEST--
Test extract() function (variation 9) 
--FILE--
<?php
/* Using Class and objects */
echo "\n*** Testing for object ***\n";
class classA
{
  public  $v;
}

$A = new classA();
var_dump ( extract(get_object_vars($A),EXTR_REFS));

echo "Done\n";
?>
--EXPECT--
*** Testing for object ***
int(1)
Done
