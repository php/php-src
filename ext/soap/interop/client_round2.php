<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html>
<head>
	<title>Round 2 Interop Client Tests</title>
</head>

<body>
<a href="index.php">Back to Interop Index</a><br>
<p>&nbsp;</p>
<?php
require_once 'SOAP/interop/client_round2_interop.php';

$iop = new Interop_Client();

function endpointList($test,$sel_endpoint)
{
    global $iop;
    $iop->getEndpoints($test);
    echo "<select name='endpoint'>\n";
    foreach ($iop->endpoints as $epname => $epinfo) {
        $selected = '';
        if ($sel_endpoint == $epname) $selected = ' SELECTED';
        echo "<option value='$epname'$selected>$epname</option>\n";
    }
    echo "</select>\n";
}
function methodList($test,$sel_method)
{
    global $iop;
    
    $ml = $iop->getMethodList($test);
    echo "<select name='method'>\n";
    foreach ($ml as $method) {
        $selected = '';
        if ($sel_method == $method) $selected = ' SELECTED';
        echo "<option value='$method'$selected>$method</option>\n";
    }
    echo "<option value='ALL'>Run All Methods</option>\n";
    echo "</select>\n";
}

function endpointTestForm($test, $endpoint, $method, $paramType, $useWSDL)
{
    global $PHP_SELF;
    if (!$test) $test = 'base';
    echo "Round 2 '$test' Selected<br>\n";
    echo "Select endpoint and method to run:<br>\n";
    echo "<form action='$PHP_SELF' method='post'>\n";
    echo "<input type='hidden' name='test' value='$test'>\n";
    endpointList($test, $endpoint);
    methodList($test, $method);
    echo "<select name='paramType'><option value='soapval'>soap value</option>";
    echo "<option value='php'".($paramType=='php'?' selected':'').">php internal type</option></select>\n";
    echo "<select name='useWSDL'><option value='0'>go Direct</option><option value='1'".($useWSDL?' selected':'').">use WSDL</option></select>\n";
    echo "<input type='submit' value='Go'>\n";
    echo "</form><br>\n";
}

function testSelectForm($selected_test = NULL)
{
    global $iop, $PHP_SELF;
    echo "Select a Round 2 test case to run:<br>\n";
    echo "<form action='$PHP_SELF' method='post'>\n";
    echo "<select name='test'>\n";
    foreach ($iop->tests as $test) {
        $selected = '';
        if ($selected_test == $test) $selected = ' SELECTED';
        echo "<option value='$test'$selected>$test</option>\n";
    }
    echo "</select>\n";
    echo "<input type='submit' value='Go'>\n";
    echo "</form><br>\n";
}

testSelectForm($_POST['test']);
endpointTestForm($_POST['test'],$_POST['endpoint'],$_POST['method'],$_POST['paramType'],$_POST['useWSDL']);

if ($_POST['test'] && array_key_exists('endpoint', $_POST) && array_key_exists('method', $_POST)) {
    // here we execute the orders
    echo "<h2>Calling {$_POST['method']} at {$_POST['endpoint']}</h2>\n";
    echo "NOTE: wire's are slightly modified to display better in web browsers.<br>\n";
    
    $iop->currentTest = $_POST['test'];      // see $tests above
    $iop->paramType = $_POST['paramType'];     // 'php' or 'soapval'
    $iop->useWSDL = $_POST['useWSDL'];           // 1= do wsdl tests
    $iop->numServers = 0;        // 0 = all
    $iop->specificEndpoint = $_POST['endpoint']; // test only this endpoint
    $iop->testMethod = $_POST['method']=='ALL'?'':$_POST['method'];       // test only this method
    $iop->skipEndpointList = array(); // endpoints to skip
    $this->nosave = 0; // 1= disable saving results to database
    // debug output
    $iop->show = 1;
    $iop->debug = 1;
    $iop->showFaults = 0; // used in result table output
    echo '<pre>';
    $iop->doTest();  // run a single set of tests using above options
    echo '</pre>';
}
?>
</body>
</html>
