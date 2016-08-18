--TEST--
There must be a difference between label: try { ... } and try { label: ... }
--FILE--
<?php

label: try {
	goto label;
} finally {
	print "success";
	return; // don't loop
}

?>
--EXPECT--
success
