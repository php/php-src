--TEST--
Bug #71826 (DateTime::diff confuse on timezone 'Asia/Tokyo')
--FILE--
<?php
// Asia/Tokyo ...something wrong
date_default_timezone_set('Asia/Tokyo');

$a = (new DateTime('2015-2-1'))->diff(new DateTime('2015-3-1'));
echo "a(Asia/Tokyo): 2015-2-1 <--> 2015-3-1\n";
var_dump($a->m, $a->d);

$b = (new DateTime('2015-3-1'))->diff(new DateTime('2015-3-29'));
echo "\nb(Asia/Tokyo): 2015-3-1 <--> 2015-3-29\n";
var_dump($b->m, $b->d);

$c = (new DateTime('2015-4-1'))->diff(new DateTime('2015-4-29'));
echo "\nc(Asia/Tokyo): 2015-4-1 <--> 2015-4-29\n";
var_dump($c->m, $c->d);
?>
--EXPECT--
a(Asia/Tokyo): 2015-2-1 <--> 2015-3-1
int(1)
int(0)

b(Asia/Tokyo): 2015-3-1 <--> 2015-3-29
int(0)
int(28)

c(Asia/Tokyo): 2015-4-1 <--> 2015-4-29
int(0)
int(28)

