<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html>
<head>
	<title>PEAR SOAP Interop</title>
</head>
<?php
// get our endpoint
$server = $_SERVER['SERVER_NAME'].':'.$_SERVER['SERVER_PORT'];
$endpoint = 'http://'.$server."/soap_interop/server_round2.php";
$base = 'http://'.$server."/soap_interop/interop.wsdl";
$groupb = 'http://'.$server."/soap_interop/interopB.wsdl";
$groupc = 'http://'.$server."/soap_interop/echoheadersvc.wsdl";
?>
<body>

<h2 align='center'>PEAR SOAP Interop</h2>
<p>Welcome to the PEAR SOAP Interop pages.  These pages are set up for
SOAP Builder interop tests.  You can find out more about the interop tests
at <a href="http://www.whitemesa.com/interop.htm">White Mesa</a>.</p>
<p>Currently Round 2 base, Group B and Group C interop tests are enabled.</p>

<h3>Round 2 Interop Server</h3>
Endpoint: <?php echo $endpoint; ?><br>
Base WSDL: <a href="<?php echo $base ?>"><?php echo $base ?></a><br>
Group B WSDL: <a href="<?php echo $groupb ?>"><?php echo $groupb ?></a><br>
Group C WSDL: <a href="<?php echo $groupc ?>"><?php echo $groupc ?></a><br>

<h3>Interop Client</h3>

<p>
Notes:
Tests are done both "Direct" and with "WSDL".  WSDL tests use the supplied interop WSDL
to run the tests against.  The Direct method uses an internal prebuilt list of methods and parameters
for the test.</p>
<p>
Tests are also run against two methods of generating method parameters.  The first, 'php', attempts
to directly serialize PHP variables into soap values.  The second method, 'soapval', uses a SOAP_Value
class to define what the type of the value is.  The second method is more interopable than the first
by nature.
</p>

<h3>Client Test Interface</h3>
<p>The <a href="client_round2.php">client interface</a> allows you to run the PEAR SOAP
Client against a choosen interop server.  Each run updates the results database below.</p>

<h3>Interop Client Test Results</h3>
<p>This is a database of the current test results using PEAR SOAP Clients against interop servers.</p>
<p>
More detail (wire) about errors (marked yellow or red) can be obtained by clicking on the
link in the result box.  If we have an HTTP error
attempting to connect to the endpoint, we will mark all consecutive attempts as errors, and skip
testing that endpoint.  This reduces the time it takes to run the tests if a server is unavailable.
WSDLCACHE errors mean we cannot retreive the WSDL file specified for the endpoint.
</p>

<ul>
<li><a href="client_round2_results.php?test=base&type=php&wsdl=0">Base results using PHP native types</a></li>
<li><a href="client_round2_results.php?test=base&type=soapval&wsdl=0">Base results using SOAP types</a></li>
<li><a href="client_round2_results.php?test=base&type=php&wsdl=1">Base results using PHP native types with WSDL</a></li>
<li><a href="client_round2_results.php?test=GroupB&type=php&wsdl=0">Group B results using PHP native types</a></li>
<li><a href="client_round2_results.php?test=GroupB&type=soapval&wsdl=0">Group B results using SOAP types</a></li>
<li><a href="client_round2_results.php?test=GroupB&type=php&wsdl=1">Group B results using PHP native types with WSDL</a></li>
<li><a href="client_round2_results.php?test=GroupC&type=php&wsdl=0">Group C results using PHP native types</a></li>
<li><a href="client_round2_results.php?test=GroupC&type=soapval&wsdl=0">Group C results using SOAP types</a></li>
<li><a href="client_round2_results.php?test=GroupC&type=php&wsdl=1">Group C results using PHP native types with WSDL</a></li>
<li><a href="client_round2_results.php">Show All Results</a></li>
</ul>
</body>
</html>
