<?php

include dirname(__FILE__)."/../OS/Guess.php";
$os =& new OS_Guess;
print $os->getSignature() . "\n";

