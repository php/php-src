<html><body>
<?
error_reporting(2039);
include("SOAP/Client.php");

$txt = "Bjoern";

$soapc = new SOAP_Client("http://localhost/soap_interop/testserver.php");
$soapc->debug_flag = TRUE;
print_r($soapc->call("testMethod",array("txt" => $txt)));
print "<br>Debug: ";
print $soapc->wire;
print "<br><br>";
unset($soapc);

?>
</html></body>