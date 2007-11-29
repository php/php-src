--TEST--
http-stream test
--FILE--
<?php
$d = new DomDocument;
$e = $d->load("http://php.net/news.rss");
echo "ALIVE\n";
?>
--EXPECTF--
ALIVE
