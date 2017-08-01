--TEST--
Test http_query_encode() function: usage variations - first arguments as object 
--CREDITS--
Adam Gegotek <adam [dot] gegotek [at] gmail [dot] com>
--FILE--
<?php

/* Prototype:   string http_query_encode(mixed $query_data [, string $options ]]] )
 * Description: Generates a URL-encoded query string from the associative (or indexed) array provided. 
 * Source code: ext/standard/http.c
*/

class UrlBuilder
{
  public $name = 'homepage';
  public $page = 1;
  protected $sort = 'desc,name';
  private $access = 'admin';
}

$obj = new stdClass;
$obj->name = 'homepage';
$obj->page = 1;
$obj->sort = 'desc,name';

echo http_query_encode($obj) . PHP_EOL;
echo http_query_encode(new UrlBuilder());
?>
--EXPECTF--
name=homepage&page=1&sort=desc%2Cname
name=homepage&page=1
