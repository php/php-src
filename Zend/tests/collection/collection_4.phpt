--TEST--
Collection: Syntax
--FILE--
<?php
collection Articles(int => Article)
{
	public function shuffle()
	{
		return array_shuffle($this->value);
	}
}

class Article
{
	public function __construct(private string $title) {}
}


$c = new Articles;
$c[] = new Article("First Test");

var_dump($c->shuffle());
?>
--EXPECTF--
object(Articles)#%d (%d) {
  ["value"]=>
  uninitialized(array)
}
