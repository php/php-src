--TEST--
Tests for floor en ceil
--FILE--
<?php
    $a = ceil (-0);   $b = ceil (-1);	$c = ceil (-1.5); 
    $d = ceil (-1.8); $e = ceil (-2.7);
    var_dump ($a, $b, $c, $d, $e);
    
    $a = ceil (0);   $b = ceil (0.5); $c = ceil (1);
    $d = ceil (1.5); $e = ceil (1.8); $f = ceil (2.7);
    var_dump ($a, $b, $c, $d, $e, $f);
    
    $a = floor (-0);   $b = floor (-0.5); $c = floor (-1);
    $d = floor (-1.5); $e = floor (-1.8); $f = floor (-2.7);
    var_dump ($a, $b, $c, $d, $e, $f);
    
    $a = floor (0);   $b = floor (0.5); $c = floor (1);
    $d = floor (1.5); $e = floor (1.8); $f = floor (2.7);
    var_dump ($a, $b, $c, $d, $e, $f);
?>  
--EXPECT--
float(0)
float(-1)
float(-1)
float(-1)
float(-2)
float(0)
float(1)
float(1)
float(2)
float(2)
float(3)
float(0)
float(-1)
float(-1)
float(-2)
float(-2)
float(-3)
float(0)
float(0)
float(1)
float(1)
float(1)
float(2) 
