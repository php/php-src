--TEST--
Test http_query_encode() function: usage variations - testing encoding
--CREDITS--
Adam Gegotek <adam [dot] gegotek [at] gmail [dot] com>
--FILE--
<?php

/* Prototype:   string http_query_encode(mixed $query_data [, string $options ]]] )
 * Description: Generates a URL-encoded query string from the associative (or indexed) array provided. 
 * Source code: ext/standard/http.c
*/

$oDimensional = array(
  "name" => "main page",
  "sort" => "desc,admin",
  "equation" => "10 + 10 - 5"
);

echo http_query_encode($oDimensional, [
    'numeric_prefix' => '',
    'arg_separator' => ini_get('arg_separator.output'),
    'encoding' => HTTP_ENCODING_RFC1738
]), PHP_EOL;
echo http_query_encode($oDimensional, [
    'numeric_prefix' => '',
    'arg_separator' => ini_get('arg_separator.output'),
    'encoding' => HTTP_ENCODING_RFC3986
]), PHP_EOL;
?>
--EXPECTF--
name=main+page&sort=desc%2Cadmin&equation=10+%2B+10+-+5
name=main%20page&sort=desc%2Cadmin&equation=10%20%2B%2010%20-%205
