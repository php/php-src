<HTML>
<HEAD>
<TITLE>Quick &amp; dirty ODBC test</TITLE>
</HEAD>
<BODY>
<H1>ODBC Test 1 - Connection</H1>
<?php
  if(isset($dbuser)){
	echo "Connecting to $dsn as $dbuser\n<P>";
	$conn = odbc_connect($dsn,$dbuser,$dbpwd);
	if(!$conn){
?>
<H2>Error connecting to database! Check DSN, username and password</H2>
<?php
    }else{
?>
<H2>Connection successful</H2>
<A HREF="odbc-t2.php<?php echo "?dbuser=$dbuser&dsn=$dsn&dbpwd=$dbpwd" ?>">Proceed to next test</A>
| <A HREF="<?php echo $PHP_SELF ?>">Change login information</A>
<?php
    }
} else {
?>
<EM>You will need permisson to create tables for the following tests!</EM>
<form action=odbc-t1.php method=post>
<table border=0>
<tr><td>Database (DSN): </td><td><input type=text name=dsn></td></tr>
<tr><td>User: </td><td><input type=text name=dbuser></td></tr>
<tr><td>Password: </td><td><input type=password name=dbpwd></td></tr>
</table>
<br>
<input type=submit value=connect>

</form>
<?php 
} ?>
</BODY>
</HTML>
