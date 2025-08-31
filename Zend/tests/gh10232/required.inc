<?php

trigger_error(basename(__FILE__));
$ex = new Exception();
echo 'Exception in ', basename($ex->getFile()), ' on line ', $ex->getLine(), "\n";
