--TEST--
Check for Yaf_Response
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php 
$response = new Yaf_Response_Http();


$body  = <<<HTML
ifjakdsljfklasdjfkljasdkljfkljadsf
HTML;

$string = "laruence";

$response->appendBody($body);
$response->prependBody($string);
$response->appendBody("kfjdaksljfklajdsfkljasdkljfkjasdf");

$body = $response->getBody();
unset($body);

debug_zval_dump($response->getBody());
unset($string);
debug_zval_dump($response->getBody());
echo $response;
debug_zval_dump($response->getBody());
$response->response();
debug_zval_dump($response->getBody());
?>
--EXPECTF--
string(75) "laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdf" refcount(1)
string(75) "laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdf" refcount(1)
laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdfstring(75) "laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdf" refcount(1)
laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdfstring(75) "laruenceifjakdsljfklasdjfkljasdkljfkljadsfkfjdaksljfklajdsfkljasdkljfkjasdf" refcount(1)
