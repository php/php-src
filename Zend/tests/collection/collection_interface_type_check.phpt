--TEST--
Collection: Type checking against interface
--FILE--
<?php
collection(Dict) Articles<int => Article>
{
}

collection(Seq) ArticleList<Article>
{
}

$c = new Articles();

$l = new ArticleList();

function needsDict(DictCollection $dict) {
    var_dump($dict);
}

function needsSeq(SeqCollection $seq) {
    var_dump($seq);
}

needsDict($c);
needsSeq($l);

?>
--EXPECTF--
object(Articles)#%d (%d) {
  ["value"]=>
  uninitialized(array)
}
object(ArticleList)#%d (%d) {
  ["value"]=>
  uninitialized(array)
}
