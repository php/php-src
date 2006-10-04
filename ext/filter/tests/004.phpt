--TEST--
GET/POST/REQUEST Test with filtered data
--SKIPIF--
<?php
/* CGI doesn't read commandline options when it sees REQUES_METHOD */
die("skip");
die("not possible to set ini setting using -d and CGI");
?>
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
