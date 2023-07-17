--TEST--
Collection: Sequence with errors
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

try {
	$c["first"] = new Article("First Test");
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
ValueError: Specifying an offset for sequence collections is not allowed
