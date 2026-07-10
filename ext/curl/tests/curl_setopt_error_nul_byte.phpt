--TEST--
curl_setopt() throws ValueError for NUL bytes in lists
--EXTENSIONS--
curl
--FILE--
<?php

$ch = curl_init();

$list_options = [
    "CURLOPT_HTTP200ALIASES",
    "CURLOPT_HTTPHEADER",
    "CURLOPT_POSTQUOTE",
    "CURLOPT_PREQUOTE",
    "CURLOPT_QUOTE",
    "CURLOPT_TELNETOPTIONS",
    "CURLOPT_MAIL_RCPT",
    "CURLOPT_RESOLVE",
    "CURLOPT_PROXYHEADER",
    "CURLOPT_CONNECT_TO",
];

foreach ($list_options as $option) {
    try {
        curl_setopt($ch, constant($option), ["Something: foo\0bar"]);
    } catch (ValueError $exception) {
        echo $option . ": " . $exception->getMessage() . "\n\n";
    }
}

$ch = null;
?>
--EXPECT--
CURLOPT_HTTP200ALIASES: curl_setopt(): cURL option CURLOPT_HTTP200ALIASES must not contain any null bytes

CURLOPT_HTTPHEADER: curl_setopt(): cURL option CURLOPT_HTTPHEADER must not contain any null bytes

CURLOPT_POSTQUOTE: curl_setopt(): cURL option CURLOPT_POSTQUOTE must not contain any null bytes

CURLOPT_PREQUOTE: curl_setopt(): cURL option CURLOPT_PREQUOTE must not contain any null bytes

CURLOPT_QUOTE: curl_setopt(): cURL option CURLOPT_QUOTE must not contain any null bytes

CURLOPT_TELNETOPTIONS: curl_setopt(): cURL option CURLOPT_TELNETOPTIONS must not contain any null bytes

CURLOPT_MAIL_RCPT: curl_setopt(): cURL option CURLOPT_MAIL_RCPT must not contain any null bytes

CURLOPT_RESOLVE: curl_setopt(): cURL option CURLOPT_RESOLVE must not contain any null bytes

CURLOPT_PROXYHEADER: curl_setopt(): cURL option CURLOPT_PROXYHEADER must not contain any null bytes

CURLOPT_CONNECT_TO: curl_setopt(): cURL option CURLOPT_CONNECT_TO must not contain any null bytes
