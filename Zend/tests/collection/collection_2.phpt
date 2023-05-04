--TEST--
Collection: Array access with integer key
--FILE--
<?php
class Article
{
	function __construct(public string $title)
	{
	}
}

collection Articles(int => Article)
{
}

$c = new Articles;
$c[9] = new Article("First Test");
$c[] = new Article("Second Test");

var_dump($c);

var_dump(isset($c[8]));
var_dump(isset($c[9]));
var_dump(isset($c[10]));

var_dump($c[10]);

unset($c[9]);
var_dump(isset($c[9]));

try {
	var_dump(isset($c["eleven"]));
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
object(Articles)#%d (%d) {
  ["value"]=>
  array(2) {
    [9]=>
    object(Article)#%d (%d) {
      ["title"]=>
      string(10) "First Test"
    }
    [10]=>
    object(Article)#%d (%d) {
      ["title"]=>
      string(11) "Second Test"
    }
  }
}
bool(false)
bool(true)
bool(true)
object(Article)#%d (%d) {
  ["title"]=>
  string(11) "Second Test"
}
bool(false)
TypeError: Key type string of element does not match collection key type int
