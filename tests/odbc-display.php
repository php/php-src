<?php
  if(($conn = odbc_connect($dsn, $dbuser, $dbpwd))){
	if(($res = odbc_do($conn, "select gif from php_test where id='$id'"))){
		odbc_binmode($res, 0);
		odbc_longreadlen($res, 0);
		if(odbc_fetch_row($res)){
			header("content-type: image/gif");
			odbc_result($res, 1);
			exit;
		}else{
			echo "Error in odbc_fetch_row";
		}
	} else {
		echo "Error in odbc_do";
	}
  } else {
	echo "Error in odbc_connect";
  }
?>
