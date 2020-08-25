--TEST--
Testing mb_convert_kana() function
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring not enabled');
if (!function_exists("mb_convert_kana")) print "skip mb_convert_kana() not available";
?>
--FILE--
<?php
$zenKakuA    =	'ァアィイゥウェエォオカガキギク';
$zenKakuB    =	'グケゲコゴサザシジスズセゼソゾタ';
$zenKakuC    =	'ダチヂッツヅテデトドナニヌネノハ';
$zenKakuD    =	'バパヒビピフブプヘベペホボポマミ';
$zenKakuE    =	'ムメモャヤュユョヨラリルレロヮワ';
$zenKakuF    =	'ヰヱヲンヴヵヶヷヸヹヺ・ーヽヾ';

$hanKakuA    =	'｠｡｢｣､･ｦｧｨｩｪｫｬｭｮｯ';
$hanKakuB    =	'ｰｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿ';
$hanKakuC    =	'ﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏ';
$hanKakuD    =	'ﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝﾞﾟ';

// Convert all Zenkaku to Hankaku; no effect
echo "'A': " . $zenKakuA . ' => ' . mb_convert_kana($zenKakuA, 'AK', 'utf-8') . "\n";
// Convert all Hankaku to Zenkaku; has an effect
echo "'a': " . $zenKakuB . ' => ' . mb_convert_kana($zenKakuB, 'ak', 'utf-8') . "\n";
echo "'a': " . $zenKakuC . ' => ' . mb_convert_kana($zenKakuC, 'ak', 'utf-8') . "\n";
echo "'a': " . $zenKakuD . ' => ' . mb_convert_kana($zenKakuD, 'ak', 'utf-8') . "\n";
echo "'a': " . $zenKakuE . ' => ' . mb_convert_kana($zenKakuE, 'ak', 'utf-8') . "\n";
echo "'a': " . $zenKakuF . ' => ' . mb_convert_kana($zenKakuF, 'ak', 'utf-8') . "\n";
echo "\n";
// Convert all Zenkaku to Hankaku; has an effect
echo "'A': " . $hanKakuA . ' => ' . mb_convert_kana($hanKakuA, 'AK', 'utf-8') . "\n";
echo "'A': " . $hanKakuB . ' => ' . mb_convert_kana($hanKakuB, 'AK', 'utf-8') . "\n";
echo "'A': " . $hanKakuC . ' => ' . mb_convert_kana($hanKakuC, 'AK', 'utf-8') . "\n";
echo "'A': " . $hanKakuD . ' => ' . mb_convert_kana($hanKakuD, 'AK', 'utf-8') . "\n";

echo "\n";
// Try combination of flags which doesn't make sense
try {
  mb_convert_kana($zenKakuA, 'HK', 'utf-8');
} catch (\ValueError $e) {
  echo $e->getMessage() . "\n";
}

// Try non-existent flag
try {
  mb_convert_kana($zenKakuA, 'Z', 'utf-8');
} catch (\ValueError $e) {
  echo $e->getMessage() . "\n";
}
?>
--EXPECT--
'A': ァアィイゥウェエォオカガキギク => ァアィイゥウェエォオカガキギク
'a': グケゲコゴサザシジスズセゼソゾタ => ｸﾞｹｹﾞｺｺﾞｻｻﾞｼｼﾞｽｽﾞｾｾﾞｿｿﾞﾀ
'a': ダチヂッツヅテデトドナニヌネノハ => ﾀﾞﾁﾁﾞｯﾂﾂﾞﾃﾃﾞﾄﾄﾞﾅﾆﾇﾈﾉﾊ
'a': バパヒビピフブプヘベペホボポマミ => ﾊﾞﾊﾟﾋﾋﾞﾋﾟﾌﾌﾞﾌﾟﾍﾍﾞﾍﾟﾎﾎﾞﾎﾟﾏﾐ
'a': ムメモャヤュユョヨラリルレロヮワ => ﾑﾒﾓｬﾔｭﾕｮﾖﾗﾘﾙﾚﾛﾜﾜ
'a': ヰヱヲンヴヵヶヷヸヹヺ・ーヽヾ => ｲｴｦﾝｳﾞヵヶヷヸヹヺ･ｰヽヾ

'A': ｠｡｢｣､･ｦｧｨｩｪｫｬｭｮｯ => ｠。「」、・ヲァィゥェォャュョッ
'A': ｰｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿ => ーアイウエオカキクケコサシスセソ
'A': ﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏ => タチツテトナニヌネノハヒフヘホマ
'A': ﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝﾞﾟ => ミムメモヤユヨラリルレロワン゛゜

mb_convert_kana(): Argument #2 ($mode) must not combine 'K' and 'H' flags
mb_convert_kana(): Argument #2 ($mode) contains invalid flag: 'Z'
--CREDITS--
Jason Easter <easter@phpug-wuerzburg.de>
PHPUG Würzburg <phpug-wuerzburg.de>
Testfest 2009 2009-06-20
