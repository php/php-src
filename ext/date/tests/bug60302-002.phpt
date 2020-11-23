--TEST--
Test for bug #60302: DateTimeImmutable::createFromFormat should new static(), not new self()
--FILE--
<?php

class MyDateTime extends DateTimeImmutable { }

$d = MyDateTime::createFromFormat('Y-m-d', '2011-01-01');

echo get_class($d);
?>
--EXPECT--
MyDateTime
