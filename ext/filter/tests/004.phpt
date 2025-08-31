--TEST--
GET/POST/REQUEST Test with filtered data
--EXTENSIONS--
filter
--INI--
filter.default=special_chars
error_reporting=E_ALL&~E_DEPRECATED
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
O&#39;Henry&#60;b&#62;Bold&#60;/b&#62;&#34;quotes&#34;\slash
O&#39;Henry&#60;b&#62;Bold&#60;/b&#62;&#34;quotes&#34;\slash
