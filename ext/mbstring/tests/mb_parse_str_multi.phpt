--TEST--
mb_parse_str() with multiple candidate encodings
--EXTENSIONS--
mbstring
--INI--
mbstring.http_input=UTF-8,SJIS,EUC-JP
--FILE--
<?php
// The encoding of the input strings will be guessed, from the list specified
// via mbstring.http_input
// All of them will be converted to UTF-8
mb_internal_encoding('UTF-8');

$queries = array(
  // UTF-8
  "テスト=abc",
  // SJIS
  "\x82\xA0\x82\xA2\x82\xA4=\x93V"
);

foreach ($queries as $query) {
  echo "Query: " . bin2hex($query) . "\n";

  $array = [];
  mb_parse_str($query, $array);

  foreach ($array as $key => $value) {
    echo bin2hex($key) . "=>" . bin2hex($value) . "\n";
  }
}

?>
--EXPECTF--
Deprecated: PHP Startup: Use of mbstring.http_input is deprecated in %s on line %d
Query: e38386e382b9e383883d616263
e38386e382b9e38388=>616263
Query: 82a082a282a43d9356
e38182e38184e38186=>e5a4a9
