--TEST--
dcgettext with LC_ALL is undefined behavior.
--EXTENSIONS--
gettext
--FILE--
<?php
try {
	dcgettext('dngettextTest', 'item', LC_ALL);
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	dcngettext('dngettextTest', 'item', 'item2', 1, LC_ALL);
} catch (ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
dcgettext(): Argument #3 ($category) cannot be LC_ALL
dcngettext(): Argument #5 ($category) cannot be LC_ALL
