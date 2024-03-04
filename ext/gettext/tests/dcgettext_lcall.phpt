--TEST--
dcgettext with LC_ALL is undefined behavior.
--EXTENSIONS--
gettext
--FILE--
<?php
var_dump(dcgettext('dngettextTest', 'item', LC_ALL));
var_dump(dcngettext('dngettextTest', 'item', 'item2', 1, LC_ALL));
?>
--EXPECT--
string(4) "item"
string(4) "item"
