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
--EXPECTF--
*** Testing for object ***

Strict Standards: Only variables should be passed by reference in %sextract_variation9.php on line 10
int(1)
Done
