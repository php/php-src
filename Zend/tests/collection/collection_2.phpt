--TEST--
Collection: Sequence
--FILE--
<?php
class Article
{
	function __construct(public string $title)
	{
	}
}

collection(Seq) Articles(Article)
{
}

$c = new Articles;
$c[] = new Article("First Test");
$c[] = new Article("Second Test");

var_dump($c);

var_dump(isset($c[0]));
var_dump(isset($c[1]));
var_dump(isset($c[2]));

var_dump($c[1]);

unset($c[1]);
var_dump(isset($c[1]));

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
    [0]=>
    object(Article)#%d (%d) {
      ["title"]=>
      string(10) "First Test"
    }
    [1]=>
    object(Article)#%d (%d) {
      ["title"]=>
      string(11) "Second Test"
    }
  }
}
bool(true)
bool(true)
bool(false)
object(Article)#%d (%d) {
  ["title"]=>
  string(11) "Second Test"
}
bool(false)
TypeError: Key type string of element does not match collection key type int
