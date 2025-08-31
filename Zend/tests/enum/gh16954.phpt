--TEST--
GH-16954: Enum to bool comparison is inconsistent
--FILE--
<?php

enum E {
    case C;
}

$true = true;
$false = false;

var_dump(E::C == true);
var_dump(E::C == $true);
var_dump(true == E::C);
var_dump($true == E::C);

var_dump(E::C != true);
var_dump(E::C != $true);
var_dump(true != E::C);
var_dump($true != E::C);

var_dump(E::C == false);
var_dump(E::C == $false);
var_dump(false == E::C);
var_dump($false == E::C);

var_dump(E::C != false);
var_dump(E::C != $false);
var_dump(false != E::C);
var_dump($false != E::C);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
