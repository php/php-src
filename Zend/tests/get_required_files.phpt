--TEST--
Check if get_required_files works
--CREDITS--
Sebastian Schürmann
sschuermann@chip.de
Testfest 2009 Munich
--FILE--
<?php
$files = get_required_files();
var_dump($files);
?>
--EXPECTF--
array(1) {
  [0]=>
  string(%d)%s
}
