--TEST--
comparing objects to other types
--FILE--
<?php

class Bar {
}

$b = new Bar;

var_dump($b == NULL);
var_dump($b != NULL);
var_dump($b == true);
var_dump($b != true);
var_dump($b == false);
var_dump($b != false);
var_dump($b == "");
var_dump($b != "");
var_dump($b == 0);
var_dump($b != 0);
var_dump($b == 1);
var_dump($b != 1);
var_dump($b == 1.0);
var_dump($b != 1.0);
var_dump($b == 1);


echo "Done\n";
?>
--EXPECTF--	
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)

Notice: Object of class Bar could not be converted to int in %s on line %d
bool(false)

Notice: Object of class Bar could not be converted to int in %s on line %d
bool(true)

Notice: Object of class Bar could not be converted to int in %s on line %d
bool(true)

Notice: Object of class Bar could not be converted to int in %s on line %d
bool(false)

Notice: Object of class Bar could not be converted to float in %s on line %d
bool(true)

Notice: Object of class Bar could not be converted to float in %s on line %d
bool(false)

Notice: Object of class Bar could not be converted to int in %s on line %d
bool(true)
Done
