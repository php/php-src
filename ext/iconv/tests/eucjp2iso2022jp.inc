<?php
/* charset=EUC-JP */

$str = "
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
日本語テキストとEnglish Text
";

$str = iconv("EUC-JP", "ISO-2022-JP", $str);
$str = base64_encode($str);
echo $str."\n";

?>
