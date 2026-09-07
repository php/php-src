--TEST--
Bug #72691 (mb_ereg_search raises a warning if a match zero-width)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
$str = 'foo';
mb_ereg_search_init($str);

mb_ereg_search('\A');
var_dump(mb_ereg_search_getpos());
var_dump(mb_ereg_search_getregs());

mb_ereg_search('\s*');
var_dump(mb_ereg_search_getpos());
var_dump(mb_ereg_search_getregs());

mb_ereg_search('\w+');
var_dump(mb_ereg_search_getpos());
var_dump(mb_ereg_search_getregs());

mb_ereg_search('\Z');
var_dump(mb_ereg_search_getpos());
var_dump(mb_ereg_search_getregs());
?>
--EXPECTF--
Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(0)

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
array(1) {
  [0]=>
  string(0) ""
}

Deprecated: Function mb_ereg_search() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(0)

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
array(1) {
  [0]=>
  string(0) ""
}

Deprecated: Function mb_ereg_search() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(3)

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
array(1) {
  [0]=>
  string(3) "foo"
}

Deprecated: Function mb_ereg_search() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(3)

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
array(1) {
  [0]=>
  string(0) ""
}
