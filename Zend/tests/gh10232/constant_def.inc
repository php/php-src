<?php

trigger_error(basename(__FILE__));
$ex = new Exception();
echo 'Exception in ', basename($ex->getFile()), ' on line ', $ex->getLine(), "\n";

require_once 'required.inc'; // The script of the same directory.

class ConstantDef
{
    const VALUE = true;
}
