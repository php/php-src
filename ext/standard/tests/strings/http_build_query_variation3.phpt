--TEST--
Test http_build_query() function: usage variations - testing four parameter added in PHP 5.4.0
--CREDITS--
Adam Gegotek <adam [dot] gegotek [at] gmail [dot] com>
--FILE--
<?php
/* Prototype  : string http_build_query ( mixed $query_data [, string $numeric_prefix [, string $arg_separator [, int $enc_type = PHP_QUERY_RFC1738 ]]] )
 * Description: Generates a URL-encoded query string from the associative (or indexed) array provided.
 * Source code: ext/standard/http.c
*/

$oDimensional = array(
  "name" => "main page",
  "sort" => "desc,admin",
  "equation" => "10 + 10 - 5"
);

echo http_build_query($oDimensional, '', ini_get('arg_separator.output'), PHP_QUERY_RFC1738) . PHP_EOL;
echo http_build_query($oDimensional, '', ini_get('arg_separator.output'), PHP_QUERY_RFC3986);
?>
--EXPECT--
name=main+page&sort=desc%2Cadmin&equation=10+%2B+10+-+5
name=main%20page&sort=desc%2Cadmin&equation=10%20%2B%2010%20-%205
