--TEST--
#44938: gettext functions crash with overlong strings
--SKIPIF--
<?php
if (!extension_loaded("gettext")) {
	die("skip\n");
}
--FILE--
<?php
$overflown = str_repeat('C', 8476509);
$msgid     = "msgid";
$domain    = "domain";
$category  = "cat";

var_dump(bindtextdomain($overflown, 'path'));

var_dump(dngettext($overflown, $msgid, $msgid, 1));
var_dump(dngettext($domain, $overflown, $msgid, 1));
var_dump(dngettext($domain, $msgid, $overflown, 1));

var_dump(gettext($overflown));

var_dump(ngettext($overflown, $msgid, -1));
var_dump(ngettext($msgid, $overflown, -1));

var_dump(dcgettext($overflown, $msgid, -1));
var_dump(dcgettext($domain, $overflown, -1));

var_dump(dcngettext($overflown, $msgid, $msgid, -1, -1));
var_dump(dcngettext($domain, $overflown, $msgid, -1, -1));
var_dump(dcngettext($domain, $msgid, $overflown, -1, -1));

var_dump(dgettext($overflown, $msgid));
var_dump(dgettext($domain, $overflown));

var_dump(textdomain($overflown));
?>
==DONE==
--EXPECTF--
Warning: bindtextdomain(): domain passed too long in %s on line %d
bool(false)

Warning: dngettext(): domain passed too long in %s on line %d
bool(false)

Warning: dngettext(): msgid1 passed too long in %s on line %d
bool(false)

Warning: dngettext(): msgid2 passed too long in %s on line %d
bool(false)

Warning: gettext(): msgid passed too long in %s on line %d
bool(false)

Warning: ngettext(): msgid1 passed too long in %s on line %d
bool(false)

Warning: ngettext(): msgid2 passed too long in %s on line %d
bool(false)

Warning: dcgettext(): domain passed too long in %s on line %d
bool(false)

Warning: dcgettext(): msgid passed too long in %s on line %d
bool(false)

Warning: dcngettext(): domain passed too long in %s on line %d
bool(false)

Warning: dcngettext(): msgid1 passed too long in %s on line %d
bool(false)

Warning: dcngettext(): msgid2 passed too long in %s on line %d
bool(false)

Warning: dgettext(): domain passed too long in %s on line %d
bool(false)

Warning: dgettext(): msgid passed too long in %s on line %d
bool(false)

Warning: textdomain(): domain passed too long in %s on line %d
bool(false)
==DONE==
