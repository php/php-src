<HTML>
<HEAD>
<TITLE>Database test #5</TITLE>
</HEAD>
<BODY>
<H1>ODBC Test 5 - Blobs</H1>
<?php
  if(!isset($gif1file) && !isset($display) || 
		($gif1file == "none" && $gif2file == "none" 
		 && $gif3file == "none")){
?>
<H2>Please select the images (gif) you want to put into the database</H2>
<FORM METHOD="POST" ACTION="<?php echo $PHP_SELF ?>" ENCTYPE="multipart/form-data">
Image 1: <INPUT TYPE="file" NAME="gif1file" VALUE="" SIZE="48"><P>
Image 2: <INPUT TYPE="file" NAME="gif2file" VALUE="" SIZE="48"><P>
Image 3: <INPUT TYPE="file" NAME="gif3file" VALUE="" SIZE="48"><P>
Blob database type name: <INPUT  TYPE="text" NAME="datatype" VALUE="LONG BYTE" SIZE="32">
<P>
<INPUT  TYPE="hidden"  name="dsn" value="<?php echo $dsn ?>">
<INPUT  TYPE="hidden"  name="dbuser" value="<?php echo $dbuser ?>">
<INPUT  TYPE="hidden"  name="dbpwd" value="<?php echo $dbpwd ?>">
<INPUT  TYPE="submit"  VALUE="Send  File(s)">
| <INPUT  TYPE="reset"  VALUE="reset">
</FORM>
</BODY>
</HTML>
<?php
	exit;
  }
 
  if(isset($dbuser)){
	echo "Connecting to $dsn as $dbuser\n";
	$conn = odbc_connect($dsn, $dbuser, $dbpwd);
	if(!$conn){
?>
<H2>Error connecting to database! Check DSN, username and password</H2>
<?php
	}else{
?>
 - OK<p>
<?php
		if(isset($display)){
			if(($res = odbc_exec($conn, 'select id from php_test'))){
				echo "<H3>Images in database</H3>";
				while(odbc_fetch_into($res, &$imgs)){
					echo "$imgs[0] : <IMG SRC=\"odbc-display.php?id=$imgs[0]&dbuser=$dbuser&dsn=$dsn&dbpwd=$dbpwd\">\n<P>";
				}
			}else{
				echo "Couldn't execute query";
			}
			echo "\n</BODY>\n</HTML>";
			exit;
		}
?>
Dropping table "php_test"
<?php
		Error_Reporting(0);
		$res = odbc_exec($conn, "drop table php_test");
		if($res){
			odbc_free_result($res);
		}
?>
 - OK<p>
Creating table "php_test":
<?php
		$res = odbc_exec($conn, "create table php_test (id char(32), gif $datatype)");
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
			odbc_free_result($info);
?>
</table>

Inserting data:
<?php
			echo "$gif1file - $gif2file - $gif3file";

			odbc_free_result($res);
			$res = odbc_prepare($conn, "insert into php_test values(?,?)");
			if($gif1file != "none"){
				$params[0] = "image1";
				$params[1] = "'$gif1file'";
				odbc_execute($res, $params);
			}
			if($gif2file != "none"){
				$params[0] = "image2";
				$params[1] = "'$gif2file'";
				odbc_execute($res, $params);
			}
			if($gif3file != "none"){
				$params[0] = "image3";
				$params[1] = "'$gif3file'";
				odbc_execute($res, $params);
			}
?>
 - OK<P>
<A HREF="<?php echo "$PHP_SELF?display=y&dbuser=$dbuser&dsn=$dsn&dbpwd=$dbpwd" ?>">Display Images</A>
<?php
		}
	}
  } else {
?>
<form action=odbc-t5.php method=post>
<table border=0>
<tr><td>Database: </td><td><input type=text name=dsn></td></tr>
<tr><td>User: </td><td><input type=text name=dbuser></td></tr>
<tr><td>Password: </td><td><input type=password name=dbpwd></td></tr>
</table>
<input type=submit value=connect>

</form>
<?php 
  }
?>
</BODY>
</HTML>
