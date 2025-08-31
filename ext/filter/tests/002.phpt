--TEST--
GET test with 2 values and an empty one
--EXTENSIONS--
filter
--GET--
a=1&b=&c=3
--FILE--
<?php echo $_GET['a'];
echo $_GET['b'];
echo $_GET['c'];
?>
--EXPECT--
13
