--TEST--
Test http_build_query() function: usage variations - first arguments as object
--CREDITS--
Adam Gegotek <adam [dot] gegotek [at] gmail [dot] com>
--FILE--
<?php
/* Prototype  : string http_build_query ( mixed $query_data [, string $numeric_prefix [, string $arg_separator [, int $enc_type = PHP_QUERY_RFC1738 ]]] )
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

echo http_build_query($obj) . PHP_EOL;
echo http_build_query(new UrlBuilder());
?>
--EXPECTF--
name=homepage&page=1&sort=desc%2Cname
name=homepage&page=1
