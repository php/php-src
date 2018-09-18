<?php
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2018 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at                              |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Shane Caraveo <Shane@Caraveo.com>                           |
// +----------------------------------------------------------------------+

set_time_limit(0);
require_once 'client_round2_interop.php';

$iop = new Interop_Client();
$iop->html = 0;

// force a fetch of endpoints, this happens regardless if no endpoints in database
$iop->fetchEndpoints();

// set some options
$iop->currentTest = 'GroupC'; // see $tests above
$iop->paramType = 'php';      // 'php' or 'soapval'
$iop->useWSDL = 1;            // 1= do wsdl tests
$iop->numServers = 0;         // 0 = all
//$iop->specificEndpoint = 'PHP ext/soap'; // test only this endpoint
//$iop->testMethod = 'echoString';         // test only this method

// endpoints to skip
//$iop->skipEndpointList = array('Apache Axis','IONA XMLBus','IONA XMLBus (CORBA)','MS SOAP ToolKit 2.0','MS SOAP ToolKit 3.0','Spheon JSOAP','SQLData SOAP Server','WASP Advanced 3.0');
$iop->startAt='';
$iop->nosave = 0; // 1= disable saving results to database
// debug output
$iop->show = 1;
$iop->debug = 0;
$iop->showFaults = 0; // used in result table output

$iop->doTest();  // run a single set of tests using above options
#$iop->doGroupTests(); // run a group of tests set in $currentTest
#$iop->doTests();  // run all tests, ignore above options
#$iop->outputTables();
echo "done\n";

?>
