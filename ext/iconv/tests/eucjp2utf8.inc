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

$str = iconv("EUC-JP", "UTF8", $str);
$str = base64_encode($str);
echo $str."\n";

?>
