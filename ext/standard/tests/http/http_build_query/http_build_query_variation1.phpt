--TEST--
Test http_build_query() function: usage variations - first arguments as object
--CREDITS--
Adam Gegotek <adam [dot] gegotek [at] gmail [dot] com>
--FILE--
<?php
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
--EXPECT--
name=homepage&page=1&sort=desc%2Cname
name=homepage&page=1
