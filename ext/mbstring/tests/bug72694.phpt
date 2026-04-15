--TEST--
Bug #72694 (mb_ereg_search_setpos does not accept a string's last position)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
mb_ereg_search_init('foo');

var_dump(mb_ereg_search_setpos(3));
var_dump(mb_ereg_search_getpos());

var_dump(mb_ereg_search('\Z'));
var_dump(mb_ereg_search_getpos());
?>
--EXPECTF--
Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(3)

Deprecated: Function mb_ereg_search() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(3)
