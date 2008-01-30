--TEST--
Confirm difference between assigning new directly and by reference.
--FILE--
<?php
  echo "Compile-time strict error message should precede this.\n";
  
  class Inc
  {
      private static $counter = 0;
      function __construct()
      {
          $this->id = ++Inc::$counter;
      }
  }
  
  $f = new Inc();
  $k =& $f;
  echo "\$f initially points to the first object:\n";
  var_dump($f);
  
  echo "Assigning new object directly to \$k affects \$f:\n";
  $k = new Inc();
  var_dump($f);
  
  echo "Assigning new object by ref to \$k removes it from \$f's reference set, so \$f is unchanged:\n";
  $k =& new Inc();
  var_dump($f);
?>
--EXPECTF--
Strict Standards: Assigning the return value of new by reference is deprecated in %s on line 23
Compile-time strict error message should precede this.
$f initially points to the first object:
object(Inc)#%d (1) {
  ["id"]=>
  int(1)
}
Assigning new object directly to $k affects $f:
object(Inc)#%d (1) {
  ["id"]=>
  int(2)
}
Assigning new object by ref to $k removes it from $f's reference set, so $f is unchanged:
object(Inc)#%d (1) {
  ["id"]=>
  int(2)
}
