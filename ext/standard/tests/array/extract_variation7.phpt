--TEST--
Test extract() function (variation 7)
--FILE--
<?php

/* EXTR_PREFIX_ALL called twice with same prefix string */
echo "\n*** Testing for EXTR_PREFIX_ALL called twice with same prefix string ***\n";
$a = array( "1" => "one", "2" => "two", "3" => "three", "4" => "four", "5" => "five" );
var_dump ( extract($a, EXTR_PREFIX_ALL, "same"));

$b = array( "f" => "fff", "1" => "one", 4 => 6, "" => "blank", 2 => "float", "F" => "FFF",
             "blank" => "", 3 => 3.7, 5 => 7, 6 => 8.6, '5' => "Five", "4name" => "jonny", "a" => NULL, NULL => 3 );
var_dump ( extract($b, EXTR_PREFIX_ALL, "same"));
var_dump ( extract($b, EXTR_PREFIX_ALL, "diff"));

echo "Done\n";
?>
--EXPECT--
*** Testing for EXTR_PREFIX_ALL called twice with same prefix string ***
int(5)
int(11)
int(11)
Done
