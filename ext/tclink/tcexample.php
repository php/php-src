<?
	// This example assumes that tclink.so has already been loaded.
	// Normally this is done by adding "extension=tclink.so" to your
	// php.ini, but you may also make a manual call to dl("tclink")
	// as well.  See tctest.php for example code.

	if (!$custid) $custid = "TestMerchant";
	if (!$password) $password = "password";
?>
<html>
<head><title>TCLink PHP Example</title></head>
<body bgcolor=white text=black>

	<form method="post" action="<?= $PHP_SELF ?>">

	<table cellspacing=1 cellpadding=3>
	<tr bgcolor=blue><th colspan=2 align=center>
		<font color=white>TrustCommerce PHP Example - TCLink ver. <?= tclink_getversion() ?>)</font>
	</th></tr>

	<tr><th align=right> CustID: </td><td> <input type="text" name="custid" value="<?= $custid ?>"> </td></tr>
	<tr><th align=right> Password: </td><td> <input type="text" name="password" value="<?= $password ?>"> </td></tr>
	<tr><th align=right> Action: </td><td> <select name="action">
		<option value="sale">Sale</option>
		<option value="preauth">Pre-Authorization</option>
		<option value="postauth">Post-Authorization</option>
		<option value="credit">Credit</option>
	</select> </td></tr>
	<tr><th align=right> Amount (in cents):</td><td> <input type="text" name="amount"> </td></tr>
	<tr bgcolor=lightgray><td colspan=2 align=center> Sales and Pre-Authorizations Only: </td></tr>
	<tr><th align=right> Card Number: </td><td> <input type="text" name="cc" size="16" maxlength="16"> </td></tr>
	<tr><th align=right> Expiration: </td>
	    <td><select name="mm"><? for ($i = 1; $i <= 12; $i++) { ?><option value="<?=sprintf("%02d", $i);?>"><?=sprintf("%02d", $i);?></option><? } ?></select>
	        <select name="yy"><? for($i = (strftime("%Y")); $i <= (strftime("%Y") + 10); $i++) { ?><option value="<?=substr(sprintf("%04d", $i),2,2);?>"><?=$i;?></option><? } ?></select><br>
	    </td></tr>
	<tr><th align=right> Cardholder Name: </td><td> <input type="text" name="name"> </td></tr>
	<tr bgcolor=lightgray><td colspan=2 align=center> Credits and Post-Authorizations Only: </td></tr>
	<tr><th align=right> Transaction ID: </td><td> <input type="text" name="transid" size="14" maxlength="14"> </td></tr>
	<tr><td colspan=2 align=center> <input type="submit" name="Action" value="Process"> </td></tr>

	<?
		if ($Action == 'Process')
		{
			$tclink['custid'] = $custid;
			$tclink['password'] = $password;
			$tclink['action'] = $action;
			if (is_numeric($amount))
				$tclink['amount'] = $amount;

			if ($action == 'sale' || $action == 'preauth')
			{
				$tclink['name'] = $name;
				$tclink['cc'] = $cc;
				$tclink['exp'] = $mm . $yy;
			}
			else if ($action == 'credit' || $action == 'postauth')
			{
				$tclink['transid'] = $transid;
			}

			$result = tclink_send($tclink);

			print "<tr><td colspan=2><hr></td></tr>";
			print "<tr bgcolor=blue><th colspan=2 align=center><font color=white>Transaction Results:</font></td></tr>";

			if ($result['transid'])
				printf("<tr><th>Transaction ID:</th><td>%s</td></tr>\n", $result['transid']);

			printf("<tr><th>Status:</td><td>%s</td></tr>\n", $tclink['status']);
			switch($tclink['status'])
			{
				case 'accepted':
				case 'approved':
					break;

				case 'decline':
				case 'rejected':
					printf("<tr><th>Decline Type:</th><td>%s</td></tr>\n", $tclink['declinetype']);
					break;

				case 'error':
					printf("<tr><th>Error Type</th><td>%s</td></tr>\n", $tclink['errortype']);
					break;

				case 'baddata':
					printf("<tr><th>Offenders:</th><td>%s</td></tr>\n", $tclink['$offenders']);
					break;
			}

			print "<tr bgcolor=lightgray><td colspan=2 align=center>All Results:</td></tr>";

			while(list($key, $value) = each($result))
				printf("<tr><th>%s</th><td>%s</td></tr>\n", $key, $value);
		}
	?>

</table>
</form>

</body>
</html>
