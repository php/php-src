--TEST--
mysqli connect
--FILE--
<?
	$user = "root";
	$passwd = "";
	$dbname = "test";
	$test = "";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("127.0.0.1", $user, $passwd);
	$test .= ($link) ? "1" : "0";
	mysqli_close($link);

	/*** test mysqli_connect localhost ***/
	$link = mysqli_connect("localhost", $user, $passwd);
	$test .= ($link) ? "1" : "0";
	mysqli_close($link);

	/*** test mysqli_connect localhost:port ***/
	$link = mysqli_connect("localhost", $user, $passwd, "", 3306);
	$test .= ($link) ? "1" : "0";
	mysqli_close($link);

	/*** test mysqli_real_connect ***/
	$link = mysqli_init();	
	$test.= (mysqli_real_connect($link, "localhost", $user, $passwd)) 
		? "1" : "0";
	mysqli_close($link);

	/*** test mysqli_real_connect with db ***/
	$link = mysqli_init();	
	$test .= (mysqli_real_connect($link, "localhost", $user, $passwd, $dbname)) 
		? "1" : "0";
	mysqli_close($link);

	/*** test mysqli_real_connect with port ***/
	$link = mysqli_init();	
	$test .= (mysqli_real_connect($link, "localhost", $user, $passwd, $dbname, 3306))
		? "1":"0";
	mysqli_close($link);

	/*** test mysqli_real_connect compressed ***/
	$link = mysqli_init();	
	$test .= (mysqli_real_connect($link, "localhost", $user, $passwd, $dbname, 0, NULL, MYSQLI_CLIENT_COMPRESS)) 
		? "1" : "0";
	mysqli_close($link);

	/* todo ssl connections */

	var_dump($test);
?>
--EXPECT--
string(7) "1111111"
