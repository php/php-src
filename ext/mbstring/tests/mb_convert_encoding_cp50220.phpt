--TEST--
mb_convert_encoding() -- converting to and from CP50220
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
mbstring.language=Japanese
--FILE--
<?php

/* Check conversion of certain Japanese characters which occurs when converting
 * wchar to CP50220; for example, there is a special encoding of a katakana
 * U followed by a voiced breath mark */
$utf8 = pack('H*', 'efbdb3efbe9e');
$converted = mb_convert_encoding($utf8, 'CP50220', 'UTF-8');
echo "Katakana U followed by voiced breath mark: " . join(unpack('H*', $converted)) . "\n";
$back_again = mb_convert_encoding($converted, 'UTF-8', 'CP50220');
echo "Converted back to UTF-8: " . join(unpack('H*', $back_again)) . "\n";

?>
--EXPECT--
Katakana U followed by voiced breath mark: 1b244225741b2842
Converted back to UTF-8: e383b4
