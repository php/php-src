--TEST--
Test http_build_query() function: usage variations - first arguments as multidimensional array and second argument present/not present
--CREDITS--
Adam Gegotek <adam [dot] gegotek [at] gmail [dot] com>
--FILE--
<?php
/* Prototype  : string http_build_query ( mixed $query_data [, string $numeric_prefix [, string $arg_separator [, int $enc_type = PHP_QUERY_RFC1738 ]]] )
 * Description: Generates a URL-encoded query string from the associative (or indexed) array provided. 
 * Source code: ext/standard/http.c
*/

$mDimensional = array(
  20, 
  5 => 13, 
  "9" => array(
    1 => "val1", 
    3 => "val2", 
    "string" => "string"
  ),
  "name" => "homepage", 
  "page" => 10,
  "sort" => array(
    "desc", 
    "admin" => array(
      "admin1", 
      "admin2" => array(
        "who" => "admin2", 
        2 => "test"
      )
    )
  )
);

echo http_build_query($mDimensional) . PHP_EOL;
echo http_build_query($mDimensional, 'prefix_');
?>
--EXPECTF--
0=20&5=13&9%5B1%5D=val1&9%5B3%5D=val2&9%5Bstring%5D=string&name=homepage&page=10&sort%5B0%5D=desc&sort%5Badmin%5D%5B0%5D=admin1&sort%5Badmin%5D%5Badmin2%5D%5Bwho%5D=admin2&sort%5Badmin%5D%5Badmin2%5D%5B2%5D=test
prefix_0=20&prefix_5=13&prefix_9%5B1%5D=val1&prefix_9%5B3%5D=val2&prefix_9%5Bstring%5D=string&name=homepage&page=10&sort%5B0%5D=desc&sort%5Badmin%5D%5B0%5D=admin1&sort%5Badmin%5D%5Badmin2%5D%5Bwho%5D=admin2&sort%5Badmin%5D%5Badmin2%5D%5B2%5D=test
