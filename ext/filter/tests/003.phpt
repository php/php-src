--TEST--
GET/POST/REQUEST Test
--EXTENSIONS--
filter
--POST--
d=4&e=5
--GET--
a=1&b=&c=3
--FILE--
<?php echo $_GET['a'];
echo $_GET['b'];
echo $_GET['c'];
echo $_POST['d'];
echo $_POST['e'];
echo "\n";
echo $_REQUEST['a'];
echo $_REQUEST['b'];
echo $_REQUEST['c'];
echo $_REQUEST['d'];
echo $_REQUEST['e'];
?>
--EXPECT--
1345
1345
