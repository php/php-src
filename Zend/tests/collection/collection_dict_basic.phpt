--TEST--
Collection: Dictionary
--FILE--
<?php
class Article
{
	function __construct(public string $title)
	{
	}
}

collection(Dict) Articles<string => Article>
{
}

$c = new Articles;
$c["nine"] = new Article("First Test");
$c["ten"] = new Article("Second Test");
$c->add("seven", new Article("Third Test"));

var_dump($c);

var_dump(isset($c["eight"]));
var_dump(isset($c["nine"]));
var_dump(isset($c["ten"]));

var_dump($c["nine"]);

unset($c["nine"]);
var_dump(isset($c["nine"]));

try {
	var_dump(isset($c[11]));
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
object(Articles)#%d (%d) {
  ["value"]=>
  array(3) {
    ["nine"]=>
    object(Article)#%d (%d) {
      ["title"]=>
      string(10) "First Test"
    }
    ["ten"]=>
    object(Article)#%d (%d) {
      ["title"]=>
      string(11) "Second Test"
    }
    ["seven"]=>
    object(Article)#%d (%d) {
      ["title"]=>
      string(10) "Third Test"
    }
  }
}
bool(false)
bool(true)
bool(true)
object(Article)#%d (%d) {
  ["title"]=>
  string(10) "First Test"
}
bool(false)
TypeError: Key type int of element does not match Articles dictionary key type string
