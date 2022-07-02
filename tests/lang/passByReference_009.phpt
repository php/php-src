--TEST--
Assignment as argument
--FILE--
<?php
    function foo(&$x, &$y) { $x = 1; echo $y ; }

    $x = 0;
    foo($x, $x); // prints 1 ..


    function foo2($x, &$y, $z)
    {
      echo $x; // 0
      echo $y; // 1
      $y = 2;
    }

    $x = 0;

    foo2($x, $x, $x = 1);
    echo $x; // 2
?>
--EXPECT--
1012
