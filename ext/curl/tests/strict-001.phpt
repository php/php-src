--TEST--
Basic strict type checks for curl_setopt()
--FILE--
<?php declare(strict_types=1);

$ch = curl_init();

$good = [
  CURLOPT_CRLF => true,
  CURLOPT_PORT => 31337,
  CURLOPT_REFERER => 'http://localhost',
  CURLOPT_CUSTOMREQUEST => null,
  CURLOPT_CUSTOMREQUEST => 'ALICE',
  CURLOPT_STDERR => STDERR,
  CURLOPT_HTTPHEADER => ['foo: bar'],
  CURLOPT_HEADERFUNCTION => function() {},
];

foreach ($good as $opt => $value) {
  var_dump(curl_setopt($ch, $opt, $value));
}

$bad = [
  CURLOPT_CRLF => 1,
  CURLOPT_PORT => false,
  CURLOPT_REFERER => new stdClass,
  CURLOPT_CUSTOMREQUEST => 123,
  CURLOPT_STDERR => 'php://stderr',
  CURLOPT_HTTPHEADER => 'Foo: Bar',
  CURLOPT_HEADERFUNCTION => 'no_sush_function',
];

foreach ($bad as $opt => $value) {
  try {
    var_dump(curl_setopt($ch, $opt, $value));
  } catch (\TypeError $e) {
    echo "Caught: ", $e->getMessage(), "\n";
  }
}
--EXPECT--

bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Caught: Argument 3 passed to curl_setopt() must be of type boolean, integer given
Caught: Argument 3 passed to curl_setopt() must be of type integer, boolean given
Caught: Argument 3 passed to curl_setopt() must be of type string, object given
Caught: Argument 3 passed to curl_setopt() must be of type string, integer given
Caught: Argument 3 passed to curl_setopt() must be of type resource, string given
Caught: Argument 3 passed to curl_setopt() must be iterable, string given
Caught: Argument 3 passed to curl_setopt() must be callable, string given