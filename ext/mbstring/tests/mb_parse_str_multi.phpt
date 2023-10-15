--TEST--
mb_parse_str() with multiple candidate encodings
--EXTENSIONS--
mbstring
--INI--
input_encoding=UTF-8,SJIS,EUC-JP,ISO-8859-1,ISO-2022-JP
--FILE--
<?php
mb_internal_encoding('UTF-8');

$queries = array(
  // UTF-8
  "テスト=abc",
  // SJIS
  "\x82\xA0\x82\xA2\x82\xA4=\x93V",
  "foo=\xE6&bar=\x97&baz=\xA5",
  // invalid ISO-2022-JP
  "a\x1F$(@=123",
  "abc=a\x1F$(@"
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
Query: e38386e382b9e383883d616263
e38386e382b9e38388=>616263
Query: 82a082a282a43d9356
e38182e38184e38186=>e5a4a9
Query: 666f6f3de6266261723d972662617a3da5
666f6f=>c3a6
626172=>c297
62617a=>c2a5
Query: 611f2428403d313233
611f242840=>313233
Query: 6162633d611f242840
616263=>611f242840
