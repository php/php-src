<?

if (!extension_loaded('pfpro')) {
  die("pfpro module is not compiled in PHP\n");
}

echo "<pre>\n\n";

echo "Payflow Pro library is version ".pfpro_version()."\n";

pfpro_init();

$transaction = array(USER	=> 'mylogin',
		     PWD	=> 'mypassword',
		     TRXTYPE	=> 'S',
		     TENDER	=> 'C',
		     AMT	=> 1.50,
		     ACCT	=> '4111111111111111',
		     EXPDATE	=> '0900'
		     );

$response = pfpro_process($transaction);

if (!$response) {
  die("Couldn't establish link to signio software.\n");
}

echo "Signio response code was ".$response[RESULT];
echo ", which means: ".$response[RESPMSG]."\n";

echo "\nDump of the transaction request ";
print_r($transaction);

echo "\nDump of the response ";
print_r($response);

pfpro_cleanup();

?>
