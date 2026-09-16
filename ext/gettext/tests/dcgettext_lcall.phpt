--TEST--
dcgettext with LC_ALL is undefined behavior.
--EXTENSIONS--
gettext
--FILE--
<?php
try {
	dcgettext('dngettextTest', 'item', LC_ALL);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	dcngettext('dngettextTest', 'item', 'item2', 1, LC_ALL);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
ValueError: dcgettext(): Argument #3 ($category) cannot be LC_ALL
ValueError: dcngettext(): Argument #5 ($category) cannot be LC_ALL
