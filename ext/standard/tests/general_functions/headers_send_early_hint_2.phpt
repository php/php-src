--TEST--
Using headers_send_early_and_clear() for HTTP early hints (extra headers)
--CGI--
--FILE--
<?php

header('HTTP/1.1 103 Early Hints');
header('Link: </style.css>; rel=preload; as=style');
headers_send_early_and_clear();
header('Location: http://example.com/');
echo "Foo\n";
?>
--EXPECTHEADERS--
Status: 103 Early Hints
Link: </style.css>; rel=preload; as=style
--EXPECT--
Status: 302 Found
Location: http://example.com/
Content-type: text/html; charset=UTF-8

Foo
