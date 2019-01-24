--TEST--
Bug #74371: strip_tags altering attributes
--FILE--
<?php

echo strip_tags('<img src="example.jpg" alt=":> :<">', '<img>') . PHP_EOL;
echo strip_tags('<img src="example.jpg" alt="\':> :<">', '<img>') . PHP_EOL;
echo strip_tags('<img src="example.jpg" alt=:><script>alert(0)</script>', '<img>') . PHP_EOL;
echo strip_tags('<img alt=< />', '<img>') . PHP_EOL;

?>
--EXPECT--
<img src="example.jpg" alt=":&gt; :&lt;">
<img src="example.jpg" alt="':&gt; :&lt;">
<img src="example.jpg" alt=:>alert(0)
<img alt=< />