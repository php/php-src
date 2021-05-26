--TEST--
Using headers_send_early_and_clear() for HTTP early hints (no extra headers)
--CGI--
--FILE--
<?php

header('HTTP/1.1 103 Early Hints');
header('Link: </style.css>; rel=preload; as=style');
headers_send_early_and_clear();
// Headers should be cleared.
var_dump(headers_list());
?>
--EXPECTHEADERS--
Status: 103 Early Hints
Link: </style.css>; rel=preload; as=style
--EXPECT--
Content-type: text/html; charset=UTF-8

array(0) {
}
