<?php

require "PEAR.php";

$err = new PEAR_Error;
var_dump(PEAR::isError($err));

?>