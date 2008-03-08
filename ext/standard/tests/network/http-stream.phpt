--TEST--
http-stream test
--SKIPIF--
<?php if (!extension_loaded("dom")) die("skip dom extension is not present"); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
$d = new DomDocument;
$e = $d->load("http://php.net/news.rss");
echo "ALIVE\n";
?>
--EXPECTF--
ALIVE
