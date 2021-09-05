--TEST--
Testing mb_convert_kana() function
--EXTENSIONS--
mbstring
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

echo $zenKakuA . ' => ' . mb_convert_kana($zenKakuA, 'AZKH', 'utf-8'), "\n";
echo $zenKakuB . ' => ' . mb_convert_kana($zenKakuB, 'azkh', 'utf-8'), "\n";
echo $zenKakuC . ' => ' . mb_convert_kana($zenKakuC, 'azkh', 'utf-8'), "\n";
echo $zenKakuD . ' => ' . mb_convert_kana($zenKakuD, 'azkh', 'utf-8'), "\n";
echo $zenKakuE . ' => ' . mb_convert_kana($zenKakuE, 'azkh', 'utf-8'), "\n";
echo $zenKakuF . ' => ' . mb_convert_kana($zenKakuF, 'azkh', 'utf-8'), "\n\n";

echo $hanKakuA . ' => ' . mb_convert_kana($hanKakuA, 'AZKH', 'utf-8'), "\n";
echo $hanKakuB . ' => ' . mb_convert_kana($hanKakuB, 'AZKH', 'utf-8'), "\n";
echo $hanKakuC . ' => ' . mb_convert_kana($hanKakuC, 'AZKH', 'utf-8'), "\n";
echo $hanKakuD . ' => ' . mb_convert_kana($hanKakuD, 'AZKH', 'utf-8'), "\n\n";

echo "Convert ASCII letter to full-width: A -> ", bin2hex(mb_convert_kana("\x00A", 'A', 'UTF-16BE')), "\n";
echo "Convert ASCII letter to full-width: A -> ", bin2hex(mb_convert_kana("\x00A", 'R', 'UTF-16BE')), "\n";
echo "Convert ASCII numeral to full-width: 1 -> ", bin2hex(mb_convert_kana("\x001", 'N', 'UTF-16BE')), "\n";
echo "Convert ASCII space to full-width: ", bin2hex(mb_convert_kana("\x00 ", 'S', 'UTF-16BE')), "\n\n";

echo "Convert hankaku kana to zenkaku kana:\n";
echo "Using 'glue' mode:\n";
echo bin2hex(mb_convert_kana("\xFF\x76\xFF\x9E", 'KV', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\xFF\x73\xFF\x9E", 'KV', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\xFF\x8A\xFF\x9F", 'KV', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\xFF\x8A\x00A", 'KV', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\xFF\x76\xFF\x9E", 'HV', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\xFF\x8A\xFF\x9F", 'HV', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\xFF\x8A\x00A", 'HV', 'UTF-16BE')), "\n";
echo "Not using 'glue' mode:\n";
echo bin2hex(mb_convert_kana("\xFF\x70", 'K', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\xFF\x70", 'H', 'UTF-16BE')), "\n\n";

echo "Convert selected punctuation/symbols to full-width and back:\n";
echo bin2hex(mb_convert_kana("\x00\x5C\x00\xA5\x00\x7E\x20\x3E\x00\x27\x00\x22", 'M', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\xFF\xE5\xFF\x3C\xFF\xE3\x20\x3E\x20\x18\x20\x19\x20\x1C\x20\x1D", 'm', 'UTF-16BE')), "\n\n";

echo "Convert various full-width characters to half-width:\n";
echo bin2hex(mb_convert_kana("\xFF\x01", 'a', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\xFF\x21", 'r', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\xFF\x10", 'n', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x00", 's', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x22\x12", 'a', 'UTF-16BE')), "\n\n";

echo "Convert full-width kana to half-width:\n";
echo bin2hex(mb_convert_kana("\x30\x41", 'h', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x01", 'h', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x02", 'h', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x0C", 'h', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x0D", 'h', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x9B", 'h', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x9C", 'h', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\xFC", 'h', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\xFB", 'h', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x01", 'k', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x02", 'k', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x0C", 'k', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x0D", 'k', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x9B", 'k', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x9C", 'k', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\xFC", 'k', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\xFB", 'k', 'UTF-16BE')), "\n";
echo "Including one which will expand to two codepoints:\n";
echo bin2hex(mb_convert_kana("\x30\x52", 'h', 'UTF-16BE')), "\n\n";

echo "Convert full-width hiragana to full-width katakana:\n";
echo bin2hex(mb_convert_kana("\x30\x41", 'C', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x9D", 'C', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\x9E", 'C', 'UTF-16BE')), "\n\n";

echo "Convert full-width katakana to full-width hiragana:\n";
echo bin2hex(mb_convert_kana("\x30\xA1", 'c', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\xFD", 'c', 'UTF-16BE')), "\n";
echo bin2hex(mb_convert_kana("\x30\xFE", 'c', 'UTF-16BE')), "\n\n";

echo bin2hex(mb_convert_kana("\x00\x00", 'A', 'UTF-16BE')), "\n";
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

Convert ASCII letter to full-width: A -> ff21
Convert ASCII letter to full-width: A -> ff21
Convert ASCII numeral to full-width: 1 -> ff11
Convert ASCII space to full-width: 3000

Convert hankaku kana to zenkaku kana:
Using 'glue' mode:
30ac
30f4
30d1
30cf0041
304c
3071
306f0041
Not using 'glue' mode:
30fc
30fc

Convert selected punctuation/symbols to full-width and back:
ffe5ffe5ffe3ffe32019201d
005c005c007e007e0027002700220022

Convert various full-width characters to half-width:
0021
0041
0030
0020
002d

Convert full-width kana to half-width:
ff67
ff64
ff61
ff62
ff63
ff9e
ff9f
ff70
ff65
ff64
ff61
ff62
ff63
ff9e
ff9f
ff70
ff65
Including one which will expand to two codepoints:
ff79ff9e

Convert full-width hiragana to full-width katakana:
30a1
30fd
30fe

Convert full-width katakana to full-width hiragana:
3041
309d
309e

0000
--CREDITS--
Jason Easter <easter@phpug-wuerzburg.de>
PHPUG Würzburg <phpug-wuerzburg.de>
Testfest 2009 2009-06-20
