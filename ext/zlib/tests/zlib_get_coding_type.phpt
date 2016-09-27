--TEST--
string zlib_get_coding_type ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.6.0") {
    die('SKIP php version so lower.');
}
?>
--FILE--
<?php
$compressionCoding = zlib_get_coding_type();
$codingType = "get zlib type error";
    
if((!$compressionCoding) || ("gzip" == $compressionCoding) || ("deflate" == $compressionCoding)){
  $codingType = "okey";
}

print($codingType);
?>
--CLEAN--
<?php
unset($compressionCoding);
unset($codingType);
?>
--EXPECT--
okey
