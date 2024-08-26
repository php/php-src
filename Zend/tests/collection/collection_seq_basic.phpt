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

collection(Seq) Articles<Article>
{
}

$c = new Articles;
$c[] = new Article("First Test");
$c[] = new Article("Second Test");

$c->add(new Article("Third Test"));

var_dump($c);

var_dump(isset($c[0]));
var_dump(isset($c[1]));
var_dump(isset($c[3]));

var_dump($c[1]);

unset($c[1]);
var_dump(isset($c[1]));
var_dump(isset($c[2]));

try {
	var_dump(isset($c["eleven"]));
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
object(Articles)#%d (%d) {
  ["value"]=>
  array(3) {
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
    [2]=>
    object(Article)#%d (%d) {
      ["title"]=>
      string(10) "Third Test"
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
bool(true)
bool(false)
TypeError: Key type string of element does not match Articles sequence key type int
