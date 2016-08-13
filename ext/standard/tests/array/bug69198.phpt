--TEST--
Bug #69198 (Compact function generate array with length but no content)
--FILE--
<?php
if (false) {
	    $willNeverBeDefined = true;
}
$result = compact('willNeverBeDefined');
var_dump($result, empty($result), $result === array(), empty($willNeverBeDefined));
--EXPECT--
array(0) {
}
bool(true)
bool(true)
bool(true)
