<HTML>
<HEAD>
<TITLE>Quick &amp; dirty ODBC test #2</TITLE>
</HEAD>
<BODY>
<H1>ODBC Test 2 - Create table</H1>
<?php
  if(isset($dbuser)){
	echo "Connecting to $dsn as $dbuser\n";
	$conn = odbc_connect($dsn,$dbuser,$dbpwd);
	if(!$conn){
?>
<H2>Error connecting to database! Check DSN, username and password</H2>
<?php
	}else{
?>
- OK<p>
Dropping table "php3_test"
<?php
		Error_Reporting(0);
		$res = odbc_exec($conn,"drop table php_test");
		if($res){
			odbc_free_result($res);
		}
?>
- OK<p>
Create table "php_test"
<?php
		error_reporting(1);
		$res = odbc_exec($conn, 'create table php_test (a char(16), b integer, c float, d varchar(128))');
		if($res){
			odbc_free_result($res);
?>
 - OK<p>
Table Info:<br>
<table>
 <tr>
  <th>Name</th>
  <th>Type</th>
  <th>Length</th>
 </tr>
<?php
			$info = odbc_exec($conn,"select * from php_test");
			$numfields = odbc_num_fields($info);

			for($i=1; $i<=$numfields; $i++){
?>
 <tr>
  <td><?php echo odbc_field_name($info, $i) ?></td>
  <td><?php echo odbc_field_type($info, $i) ?></td>
  <td><?php echo odbc_field_len($info,$i) ?></td>
 </tr>
<?php
			}
?>
</table>
<P>
<HR width=50%">
<P>
<A HREF="odbc-t3.php<?php echo "?dbuser=$dbuser&dsn=$dsn&dbpwd=$dbpwd" ?>">Proceed to next test</A> 
| <A HREF="<?php echo $PHP_SELF; ?>">Change login information</A>

<?php
		}
	}
  } else {
?>

<form action="odbc-t3.php" method=post>
<table border=0>
<tr><td>Database (DSN): </td><td><input type=text name=dsn></td></tr>
<tr><td>User: </td><td><input type=text name=dbuser></td></tr>
<tr><td>Password: </td><td><input type=password name=dbpwd></td></tr>
</table>
<input type=submit value="Continue">
</form>

<?php 
  } 
?>
</BODY>
</HTML>
