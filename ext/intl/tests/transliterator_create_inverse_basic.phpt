--TEST--
Transliterator::createInverse (basic)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);

$tr = Transliterator::create("Katakana-Latin");
$orstr = "オーシャンビュー";
$new_str = $tr->transliterate($orstr);

$revtr = $tr->createInverse();
$recovstr = $revtr->transliterate($new_str);

$revtr2 = transliterator_create_inverse($tr);
$recovstr2 = $revtr2->transliterate($new_str);

echo $orstr,"\n";
echo $new_str,"\n";
echo $recovstr,"\n";

var_dump(($orstr == $recovstr) == $recovstr2);

echo "Done.\n";
--EXPECT--
オーシャンビュー
ōshanbyū
オーシャンビュー
bool(true)
Done.
