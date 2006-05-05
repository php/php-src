--TEST--
GET/POST/REQUEST Test with filtered data
--INI--
filter.default=special_chars
--POST--
d="quotes"&e=\slash
--GET--
a=O'Henry&b=&c=<b>Bold</b>
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
O&#39;HenryBold&quot;quotes&quot;\slash
O&#39;HenryBold&quot;quotes&quot;\slash
