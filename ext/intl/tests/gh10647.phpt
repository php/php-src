--TEST--
Bug GH-10647 (Spoofchecker::isSuspicious $errorCode always null)
--EXTENSIONS--
intl
--FILE--
<?php
$error = 123;
var_dump($error);

$x = new Spoofchecker();
var_dump($x->isSuspicious("\u{041F}aypal.com", $error));
var_dump($error);

var_dump($x->areConfusable('google.com', 'goog1e.com', $error));
var_dump($error);
?>
--EXPECTF--
int(123)
bool(true)
int(%d)
bool(true)
int(%d)
