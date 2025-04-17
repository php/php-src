--TEST--
Test parse_url() function: Checks URL schemes should start with alpha characters.
--FILE--
<?php
$urls = [
  'valid-scheme://example.org',
  '0invalid-scheme://example.org',
  '-invalid-scheme://example.org',
  '+invalid-scheme://example.org',
  '.invalid-scheme://example.org'
];
foreach ($urls as $url) {
  echo "parse {$url}\n";
  var_dump(parse_url($url, PHP_URL_SCHEME));
}
?>
--EXPECT--
parse valid-scheme://example.org
string(12) "valid-scheme"
parse 0invalid-scheme://example.org
NULL
parse -invalid-scheme://example.org
NULL
parse +invalid-scheme://example.org
NULL
parse .invalid-scheme://example.org
NULL
