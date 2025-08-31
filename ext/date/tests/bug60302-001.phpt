--TEST--
Test for bug #60302: DateTime::createFromFormat should new static(), not new self()
--FILE--
<?php

class MyDateTime extends DateTime { }

$d = MyDateTime::createFromFormat('Y-m-d', '2011-01-01');

echo get_class($d);
?>
--EXPECT--
MyDateTime
