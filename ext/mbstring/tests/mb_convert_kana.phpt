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


echo $zenKakuA . ' => ' . mb_convert_kana($zenKakuA, 'AZKH', 'utf-8');
echo "\n";
echo $zenKakuB . ' => ' . mb_convert_kana($zenKakuB, 'azkh', 'utf-8');
echo "\n";
echo $zenKakuC . ' => ' . mb_convert_kana($zenKakuC, 'azkh', 'utf-8');
echo "\n";
echo $zenKakuD . ' => ' . mb_convert_kana($zenKakuD, 'azkh', 'utf-8');
echo "\n";
echo $zenKakuE . ' => ' . mb_convert_kana($zenKakuE, 'azkh', 'utf-8');
echo "\n";
echo $zenKakuF . ' => ' . mb_convert_kana($zenKakuF, 'azkh', 'utf-8');
echo "\n";
echo "\n";
echo $hanKakuA . ' => ' . mb_convert_kana($hanKakuA, 'AZKH', 'utf-8');
echo "\n";
echo $hanKakuB . ' => ' . mb_convert_kana($hanKakuB, 'AZKH', 'utf-8');
echo "\n";
echo $hanKakuC . ' => ' . mb_convert_kana($hanKakuC, 'AZKH', 'utf-8');
echo "\n";
echo $hanKakuD . ' => ' . mb_convert_kana($hanKakuD, 'AZKH', 'utf-8');
?>
--EXPECT--
ァアィイゥウェエォオカガキギク => ァアィイゥウェエォオカガキギク
グケゲコゴサザシジスズセゼソゾタ => ｸﾞｹｹﾞｺｺﾞｻｻﾞｼｼﾞｽｽﾞｾｾﾞｿｿﾞﾀ
ダチヂッツヅテデトドナニヌネノハ => ﾀﾞﾁﾁﾞｯﾂﾂﾞﾃﾃﾞﾄﾄﾞﾅﾆﾇﾈﾉﾊ
バパヒビピフブプヘベペホボポマミ => ﾊﾞﾊﾟﾋﾋﾞﾋﾟﾌﾌﾞﾌﾟﾍﾍﾞﾍﾟﾎﾎﾞﾎﾟﾏﾐ
ムメモャヤュユョヨラリルレロヮワ => ﾑﾒﾓｬﾔｭﾕｮﾖﾗﾘﾙﾚﾛﾜﾜ
ヰヱヲンヴヵヶヷヸヹヺ・ーヽヾ => ｲｴｦﾝｳﾞヵヶヷヸヹヺ･ｰヽヾ

｠｡｢｣､･ｦｧｨｩｪｫｬｭｮｯ => ｠。「」、・ヲァィゥェォャュョッ
ｰｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿ => ーアイウエオカキクケコサシスセソ
ﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏ => タチツテトナニヌネノハヒフヘホマ
ﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝﾞﾟ => ミムメモヤユヨラリルレロワン゛゜

--CREDITS--
Jason Easter <easter@phpug-wuerzburg.de>
PHPUG Würzburg <phpug-wuerzburg.de>
Testfest 2009 2009-06-20
