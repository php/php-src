--TEST--
Test open_basedir configuration
--INI--
open_basedir={PWD}
--FILE--
<?php
$thisdir = dirname(__FILE__);

var_dump(touch($thisdir . "afile"));

?>
