--TEST--
http-stream test
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (!extension_loaded("dom")) die("skip dom extension is not present");
require __DIR__.'/../http/server.inc';
http_server_skipif();
--INI--
allow_url_fopen=1
--FILE--
<?php
require __DIR__.'/../http/server.inc';

['pid' => $pid, 'uri' => $uri] = http_server([__DIR__."/news.rss"]);

$d = new DomDocument;
$e = $d->load("$uri/news.rss");
echo "ALIVE\n";
http_server_kill($pid);
--EXPECT--
ALIVE
