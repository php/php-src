<?
	// The ugly bit of code below loads the TCLink extension if it is not
	// already.  You can skip all of this if "extension=tclink.so" is in
	// your php.ini, or you can make a single call to dl("tclink") if
	// tclink.so is in your PHP extensions path ("extension_dir").
	if (!extension_loaded("tclink"))
	{
		$extfname = getcwd() . "/modules/tclink.so";
		echo "TCLink extension not loaded, attempting to load manually from $extfname...";

		// The excessive "../" bit allows us to back up out of the extension dir
		// so that we can access the current working directory.  It is only done
		// this way so that tctest.php may work out of the box on any system, you
		// can get rid of this if you do a global install of TCLink.
		$extfname = "../../../../../../../../../../../.." . $extfname;
		if (!dl($extfname)) {
			echo "FAILED!\nAborting this test script, please check to make sure the module is properly built.\n";
			exit(1);
		}
		echo "success.\n";
	}

	print "\nTCLink version " . tclink_getversion() . "\n";
	print "Sending transaction...";

	// Build a hash containing our parameters
	$params{'custid'} = 'TestMerchant';
	$params{'password'} = 'password';
	$params{'action'} = 'sale';
	$params{'media'} = 'cc';
	$params{'cc'} = '4111111111111111';
	$params{'exp'} = '0110';
	$params{'amount'} = '100';
	$params{'name'} = 'Joe PHP';

	// Send the hash to TrustCommerce for processing
	$result = tclink_send($params);

	print "done!\n\nTransaction results:\n";

	// Print out all parameters returned
	while (list($key, $val) = each($result))
		print "\t$key=$val\n";
?>

