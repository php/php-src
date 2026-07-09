--TEST--
curl_setopt() throws ValueError for newlines in headers
--EXTENSIONS--
curl
--FILE--
<?php

$ch = curl_init();

$bad_headers = [
    "LF injection" => ["Hello: world\nFoo: bar"],
    "CR injection" => ["Hello: world\rFoo: bar"],
    "CRLF injection" => ["Hello: world\r\nFoo: bar"]
];

foreach ($bad_headers as $type => $headers) {
    echo "Testing $type:\n";
    try {
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n\n";
    }
}

$ch = null;
?>
--EXPECT--
Testing LF injection:
curl_setopt(): Header entries for the CURLOPT_HTTPHEADER option may not contain more than a single header, new line detected

Testing CR injection:
curl_setopt(): Header entries for the CURLOPT_HTTPHEADER option may not contain more than a single header, new line detected

Testing CRLF injection:
curl_setopt(): Header entries for the CURLOPT_HTTPHEADER option may not contain more than a single header, new line detected
