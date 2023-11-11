--TEST--
mb_parse_str() error handling
--EXTENSIONS--
mbstring
--FILE--
<?php
mb_internal_encoding('UTF-8');

$queries = array(
  "\x80\x80\x80",
  "\xFF=\xFF"
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
--EXPECT--
Query: 808080
3f3f3f=>
Query: ff3dff
3f=>3f
