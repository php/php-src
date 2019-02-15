--TEST--
mb_preferred_mime_name()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
// TODO: Add more encoding names

$str = mb_preferred_mime_name('sjis-win');
echo "$str\n";

$str = mb_preferred_mime_name('SJIS');
echo "$str\n";

$str = mb_preferred_mime_name('EUC-JP');
echo "$str\n";

$str = mb_preferred_mime_name('UTF-8');
echo "$str\n";

$str = mb_preferred_mime_name('ISO-2022-JP');
echo "$str\n";

$str = mb_preferred_mime_name('JIS');
echo "$str\n";

$str = mb_preferred_mime_name('ISO-8859-1');
echo "$str\n";

$str = mb_preferred_mime_name('UCS2');
echo "$str\n";

$str = mb_preferred_mime_name('UCS4');
echo "$str\n";

echo "== INVALID PARAMETER ==\n";
// Invalid name
var_dump(mb_preferred_mime_name('BAD_NAME'));

// No preferred name
var_dump(mb_preferred_mime_name('pass'));
?>
--EXPECTF--
Shift_JIS
Shift_JIS
EUC-JP
UTF-8
ISO-2022-JP
ISO-2022-JP
ISO-8859-1
UCS-2
UCS-4
== INVALID PARAMETER ==

Warning: mb_preferred_mime_name(): Unknown encoding "BAD_NAME" in %s on line %d
bool(false)

Warning: mb_preferred_mime_name(): No MIME preferred name corresponding to "pass" in %s on line %d
bool(false)
