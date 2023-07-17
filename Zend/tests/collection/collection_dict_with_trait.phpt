--TEST--
Collection with Trait
--FILE--
<?php
trait Shuffler
{
	public function shuffle()
	{
		$values = $this->value;
		shuffle($values);

		return $values;
	}
}


collection(Dict) Articles<int => Article>
{
	use Shuffler;
}

class Article
{
	public function __construct(private string $title) {}
}


$c = new Articles;
$c[0] = new Article("First Test");

var_dump($c->shuffle());
?>
--EXPECTF--
array(1) {
  [0]=>
  object(Article)#%d (%d) {
    ["title":"Article":private]=>
    string(10) "First Test"
  }
}
