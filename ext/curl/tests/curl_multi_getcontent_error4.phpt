--TEST--
Curl_multi_getcontent() error test  with undefined handle
--CREDITS--
Rein Velt (rein@velt.org)
#TestFest Utrecht 20090509
--SKIPIF--
<?php
if (!extension_loaded('curl')) print 'skip';
?>
--FILE--
<?php
	//CURL_MULTI_GETCONTENT TEST

	//CREATE RESOURCES
 	//$ch1=undefined;
	$ch2=curl_init();

	//SET URL AND OTHER OPTIONS
	curl_setopt($ch1, CURLOPT_URL, "file://".dirname(__FILE__). DIRECTORY_SEPARATOR . "curl_testdata1.txt");
	curl_setopt($ch2, CURLOPT_URL, "file://".dirname(__FILE__). DIRECTORY_SEPARATOR . "curl_testdata2.txt");
	curl_setopt($ch1, CURLOPT_RETURNTRANSFER, true);
	curl_setopt($ch2, CURLOPT_RETURNTRANSFER, true);
	
	//CREATE MULTIPLE CURL HANDLE
	$mh=curl_multi_init();

	//ADD THE 2 HANDLES
	curl_multi_add_handle($mh,$ch1);
	curl_multi_add_handle($mh,$ch2);

	//EXECUTE
	$running=0;
	do {
		curl_multi_exec($mh,$running);
	} while ($running>0);


	$results1=curl_multi_getcontent($ch1); //incorrect parameter type
        $results2=curl_multi_getcontent($ch2);

	//CLOSE
 	//curl_multi_remove_handle($mh,$ch1);
	curl_multi_remove_handle($mh,$ch2);
	curl_multi_close($mh);	

	echo $results1; 
	echo $results2;

?>
--EXPECTF--
Notice: Undefined variable: ch1 in %s on line %d

Warning: curl_setopt() expects parameter 1 to be resource, null given in %s on line %d

Notice: Undefined variable: ch1 in %s on line %d

Warning: curl_setopt() expects parameter 1 to be resource, null given in %s on line %d

Notice: Undefined variable: ch1 in %s on line %d

Warning: curl_multi_add_handle() expects parameter 2 to be resource, null given in %s on line %d

Notice: Undefined variable: ch1 in %s on line %d

Warning: curl_multi_getcontent() expects parameter 1 to be resource, null given in %s on line %d
CURL2
