<?php // vim600: syn=php
error_reporting(E_ALL);
$utf = implode('', file('ext/iconv/tests/Quotes.UTF-8'));

print(iconv("UTF-8", "ISO-8859-1//TRANSLIT", $utf));
print(iconv("UTF-8", "ASCII//TRANSLIT", $utf));
?>
