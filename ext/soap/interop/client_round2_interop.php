<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Shane Caraveo <Shane@Caraveo.com>                           |
// +----------------------------------------------------------------------+
//
// $Id$
//
require_once 'DB.php'; // PEAR/DB
require_once 'base.php';
require_once 'client_round2_params.php';
require_once 'test.utility.php';

error_reporting(E_ALL ^ E_NOTICE);

class Interop_Client extends Interop_Base
{
    // database DNS
    var $DSN = 'mysql://user@localhost/interop2';

    // our central interop server, where we can get the list of endpoints
    var $interopServer = "http://www.whitemesa.net/interopInfo";
    
    // our local endpoint, will always get added to the database for all tests
    var $localEndpoint;
    
    // specify testing
    var $currentTest = 'base';      // see $tests above
    var $paramType = 'php';     // 'php' or 'soapval'
    var $useWSDL = 0;           // 1= do wsdl tests
    var $numServers = 0;        // 0 = all
    var $specificEndpoint = ''; // test only this endpoint
    var $testMethod = '';       // test only this method
    var $skipEndpointList = array(); // endpoints to skip
    var $nosave = 0;
    var $startAt = ''; // start in list at this endpoint
    // debug output
    var $show = 1;
    var $debug = 0;
    var $showFaults = 0; // used in result table output
    
    // PRIVATE VARIABLES
    var $dbc = NULL;
    var $totals = array();
    var $tests = array('base','GroupB', 'GroupC');
    var $paramTypes = array('php', 'soapval');
    var $endpoints = array();
    
    function Interop_Client() {
        // set up the database connection
        $this->dbc = DB::connect($this->DSN, true);
        // if it errors out, just ignore it and rely on regular methods
        if (DB::isError($this->dbc)) {
            echo $this->dbc->getMessage();
            $this->dbc = NULL;
        }
        // set up local endpoint
        $this->localEndpoint['base'] = array(
                                'endpointName'=>'PEAR SOAP',
                                'endpointURL'=>'http://localhost/soap_interop/server_round2.php',
                                'wsdlURL'=>'http://localhost/soap_interop/interop.wsdl'
                              );
        $this->localEndpoint['GroupB'] = array(
                                'endpointName'=>'PEAR SOAP',
                                'endpointURL'=>'http://localhost/soap_interop/server_round2.php',
                                'wsdlURL'=>'http://localhost/soap_interop/interopB.wsdl'
                              );
        $this->localEndpoint['GroupC'] = array(
                                'endpointName'=>'PEAR SOAP',
                                'endpointURL'=>'http://localhost/soap_interop/server_round2.php',
                                'wsdlURL'=>'http://localhost/soap_interop/echoheadersvc.wsdl'
                              );
    }
    
    function _fetchEndpoints(&$soapclient, $test) {
        $this->_getEndpoints($test, 1);
        
        // retreive endpoints from the endpoint server
        $endpointArray = $soapclient->call("GetEndpointInfo",array("groupName"=>$test),"http://soapinterop.org/info/","http://soapinterop.org/info/");
        if (PEAR::isError($endpointArray)) {
            print $soapclient->wire;
            print_r($endpointArray);
            return;
        }
        
        // add our local endpoint
        if ($this->localEndpoint[$test]) {
        array_push($endpointArray, $this->localEndpoint[$test]);
        }
        
        if (!$endpointArray) return;
        
        // reset the status to zero
        $res = $this->dbc->query("update endpoints set status = 0 where class='$test'");
        if (DB::isError($res)) {
            die ($res->getMessage());
        }
        if (is_object($res)) $res->free();
        // save new endpoints into database
        foreach($endpointArray as $k => $v){
            if (array_key_exists($v['endpointName'],$this->endpoints)) {
                $res = $this->dbc->query("update endpoints set endpointURL='{$v['endpointURL']}', wsdlURL='{$v['wsdlURL']}', status=1 where id={$this->endpoints[$v['endpointName']]['id']}");
            } else {
                $res = $this->dbc->query("insert into endpoints (endpointName,endpointURL,wsdlURL,class) values('{$v['endpointName']}','{$v['endpointURL']}','{$v['wsdlURL']}','$test')");
            }
            if (DB::isError($res)) {
                die ($res->getMessage());
            }
            if (is_object($res)) $res->free();
        }
    }
    
    /**
    *  fetchEndpoints
    * retreive endpoints interop server
    *
    * @return boolean result
    * @access private
    */    
    function fetchEndpoints($test = NULL) {
        // fetch from the interop server
        $soapclient = new SoapObject($this->interopServer);
        
        if ($test) {
            $this->_fetchEndpoints($soapclient, $test);
        } else {
            foreach ($this->tests as $test) {
                $this->_fetchEndpoints($soapclient, $test);
            }
            $test = 'base';
        }
        
        // retreive all endpoints now
        $this->currentTest = $test;
        return $this->_getEndpoints($test);
    }
    
    /**
    *  getEndpoints
    * retreive endpoints from either database or interop server
    *
    * @param string base (see local var $tests)
    * @param boolean all (if false, only get valid endpoints, status=1)
    * @return boolean result
    * @access private
    */    
    function getEndpoints($base = 'base', $all = 0) {
        if (!$this->_getEndpoints($base, $all)) {
            return $this->fetchEndpoints($base);
        }
        return TRUE;
    }

    /**
    *  _getEndpoints
    * retreive endpoints from database
    *
    * @param string base (see local var $tests)
    * @param boolean all (if false, only get valid endpoints, status=1)
    * @return boolean result
    * @access private
    */    
    function _getEndpoints($base = "", $all = 0) {
        $this->endpoints = array();

        // build sql
        $sql = "select * from endpoints ";
        if ($base) {
            $sql .= "where class='$base' ";
            if (!$all) $sql .= "and status=1";
        } else
        if (!$all) $sql .= "where status=1";
        
        $db_ep = $this->dbc->getAll($sql,NULL, DB_FETCHMODE_ASSOC );
        if (DB::isError($db_ep)) {
            echo $sql."\n";
            echo $db_ep->getMessage();
            return FALSE;
        }
        // rearange the array
        foreach ($db_ep as $entry) {
            $this->endpoints[$entry['endpointName']] = $entry;
        }
        if (count($this->endpoints) > 0) {
            $this->currentTest = $base;
            return TRUE;
        }
        return FALSE;
    }

    /**
    *  getResults
    * retreive results from the database, stuff them into the endpoint array
    *
    * @access private
    */
    function getResults($test = 'base', $type = 'php', $wsdl = 0) {
        // be sure we have the right endpoints for this test result
        $this->getEndpoints($test);
        
        // retreive the results and put them into the endpoint info
        $sql = "select * from results where class='$test' and type='$type' and wsdl=$wsdl";
        $results = $this->dbc->getAll($sql,NULL, DB_FETCHMODE_ASSOC );
        foreach ($results as $result) {
            // find the endpoint
            foreach ($this->endpoints as $epn => $epi) {
                if ($epi['id'] == $result['endpoint']) {
                    // store the info
                    $this->endpoints[$epn]['methods'][$result['function']] = $result;
                    break;
                }
            }
        }
    }
    
    /**
    *  saveResults
    * save the results of a method test into the database
    *
    * @access private
    */
    function _saveResults($endpoint_id, &$soap_test) {
        if ($this->nosave) return;
        
        $result = $soap_test->result;
        $wire = $result['wire'];
        if ($result['success']) {
            $success = 'OK';
            $error = '';
        } else {
            $success = $result['fault']->faultcode;
            $error = $result['fault']->faultstring;
            if (!$wire) $wire= $result['fault']->detail;
        }
        
        $test_name = $soap_test->test_name;
        // add header info to the test name
        if ($soap_test->headers) {
            foreach ($soap_test->headers as $h) {
                $destination = 0;
                if (get_class($h) == 'soap_header') {
                    if ($h->attributes['SOAP-ENV:actor'] == 'http://schemas.xmlsoap.org/soap/actor/next') $destination = 1;
                    $test_name .= ":{$h->name},$destination,{$h->attributes['SOAP-ENV:mustUnderstand']}";
                } else {
                    if (!$h[3] || $h[3] == 'http://schemas.xmlsoap.org/soap/actor/next') $destination = 1;
                    if (!$h[2]) $h[2] = 0;
                    $qn = new QName($h[0]);
                    $test_name .= ":{$qn->name},$destination,".(int)$h[2];
                }
            }
        }
        
        $sql = "delete from results where endpoint=$endpoint_id ".
                    "and class='$this->currentTest' and type='$this->paramType' ".
                    "and wsdl=$this->useWSDL and function=".
                    $this->dbc->quote($test_name);
        #echo "\n".$sql;
        $res = $this->dbc->query($sql);
        if (DB::isError($res)) {
            die ($res->getMessage());
        }
        if (is_object($res)) $res->free();
        
        $sql = "insert into results (endpoint,stamp,class,type,wsdl,function,result,error,wire) ".
                    "values($endpoint_id,".time().",'$this->currentTest',".
                    "'$this->paramType',$this->useWSDL,".
                    $this->dbc->quote($test_name).",".
                    $this->dbc->quote($success).",".
                    $this->dbc->quote($error).",".
                    ($wire?$this->dbc->quote($wire):"''").")";
        #echo "\n".$sql;
        $res = $this->dbc->query($sql);
        
        if (DB::isError($res)) {
            die ($res->getMessage());
        }
        if (is_object($res)) $res->free();
    }

    /**
    *  decodeSoapval
    * decodes a soap value to php type, used for test result comparisions
    *
    * @param SOAP_Value soapval
    * @return mixed result
    * @access public
    */    
    function decodeSoapval($soapval)
    {
        if (gettype($soapval) == "object" &&
            (strcasecmp(get_class($soapval),"soapparam") == 0 ||
             strcasecmp(get_class($soapval),"soapvar") == 0)) {
                if (strcasecmp(get_class($soapval),"soapparam") == 0)
                    $val = $soapval->param_data->enc_value;
                else
                    $val = $soapval->enc_value;
        } else {
            $val = $soapval;
        }
        if (is_array($val)) {
            foreach($val as $k => $v) {
                if (gettype($v) == "object" &&
                    (strcasecmp(get_class($soapval),"soapparam") == 0 ||
                    strcasecmp(get_class($soapval),"soapvar") == 0)) {
                    $val[$k] = $this->decodeSoapval($v);
                }
            }
        }
        return $val;
    }
    
    /**
    *  compareResult
    * compare two php types for a match
    *
    * @param string expect
    * @param string test_result
    * @return boolean result
    * @access public
    */    
    function compareResult($expect, $result, $type = NULL)
    {
        $ok = 0;
        $expect_type = gettype($expect);
        $result_type = gettype($result);
        if ($expect_type == "array" && $result_type == "array") {
            # compare arrays
            $ok = array_compare($expect, $result);
        } else {
            if ($type == 'boolean')
                $ok = boolean_compare($expect, $result);
            else
                $ok = string_compare($expect, $result);
        }
        return $ok;
    }


    /**
    *  doEndpointMethod
    *  run a method on an endpoint and store it's results to the database
    *
    * @param array endpoint_info
    * @param SOAP_Test test
    * @return boolean result
    * @access public
    */    
    function doEndpointMethod(&$endpoint_info, &$soap_test) {
        $ok = FALSE;
        
        // prepare a holder for the test results
        $soap_test->result['class'] = $this->currentTest;
        $soap_test->result['type'] = $this->paramType;
        $soap_test->result['wsdl'] = $this->useWSDL;
        
        if ($this->useWSDL) {
            if (array_key_exists('wsdlURL',$endpoint_info)) {
                if (!array_key_exists('client',$endpoint_info)) {
                    $endpoint_info['client'] = new SoapObject($endpoint_info['wsdlURL']);
                }
                $soap =& $endpoint_info['client'];
                
                # XXX how do we determine a failure on retreiving/parsing wsdl?
                if ($soap->wsdl->fault) {
                    $fault = $soap->wsdl->fault->getFault();
                    $soap_test->setResult(0,'WSDL',
                                          $fault->faultstring."\n\n".$fault->detail,
                                          $fault->faultstring,
                                          $fault
                                          );
                    return FALSE;
                }
            } else {
                $fault = new SoapFault('WSDL',"no WSDL defined for $endpoint");
                $soap_test->setResult(0,'WSDL',
                                      $fault->faultstring,
                                      $fault->faultstring,
                                      $fault
                                      );
                return FALSE;
            }
            $namespace = false;
            $soapaction = false;
        } else {
            $namespace = $soapaction = 'http://soapinterop.org/';
            // hack to make tests work with MS SoapToolkit
            // it's the only one that uses this soapaction, and breaks if
            // it isn't right.  Can't wait for soapaction to be fully depricated
            if ($this->currentTest == 'base' &&
                strstr($endpoint_info['endpointName'],'MS SOAP ToolKit 2.0')) {
                $soapaction = 'urn:soapinterop';
            }
            if (!array_key_exists('client',$endpoint_info)) {
                $endpoint_info['client'] = new SoapObject($endpoint_info['endpointURL'],$soapaction);
            }
            $soap = $endpoint_info['client'];
        }
        // add headers to the test
        if ($soap_test->headers) {
            // $header is already a SOAP_Header class
            foreach ($soap_test->headers as $header) {
                $soap->addHeader($header);
            }
        }
        // XXX no way to set encoding
        // this lets us set UTF-8, US-ASCII or other
        //$soap->setEncoding($soap_test->encoding);
        
        if ($this->useWSDL) {
            $args = '';
            foreach ($soap_test->method_params as $pname => $param) {
                $arg = '$soap_test->method_params["'.$pname.'"]';
                $args .= $args?','.$arg:$arg;
            }
            $return = eval('return $soap->'.$soap_test->method_name.'('.$args.');');
        } else {
            $return = $soap->__call($soap_test->method_name,$soap_test->method_params,$soapaction, $namespace);
        }
        
        // save the wire
        $wire = $soap->__getlastrequest()."\n\n".$soap->__getlastresponse();
        $wire = str_replace('>',">\n",$wire);
        $wire = str_replace('" ',"\" \n",$wire);
        #print $wire;
        
        if(!$soap->__isfault()){
            if (is_array($soap_test->method_params) && count($soap_test->method_params) == 1) {
                $sent = array_shift($soap_test->method_params);
            } else {
                $sent = $soap_test->method_params;
            }

            // compare header results
            $header_result = array();
            $headers_ok = TRUE;
            
            # XXX need to implement header support!
            #
            #if ($soap_test->headers) {
            #    // $header is already a SOAP_Header class
            #    foreach ($soap_test->headers as $header) {
            #        if (get_class($header) != 'soap_header') {
            #            // assume it's an array
            #            $header = new SOAP_Header($header[0], NULL, $header[1], $header[2], $header[3], $header[4]);
            #        }
            #        $expect = $soap_test->headers_expect[$header->name];
            #        $header_result[$header->name] = array();
            #        // XXX need to fix need_result to identify the actor correctly
            #        $need_result = $hresult ||
            #            ($header->attributes['SOAP-ENV:actor'] == 'http://schemas.xmlsoap.org/soap/actor/next'
            #             && $header->attributes['SOAP-ENV:mustUnderstand']);
            #        if ($expect) {
            #            $hresult = $soap->headers[key($expect)];
            #            $ok = !$need_result || $this->compareResult($hresult ,$expect[key($expect)]);
            #        } else {
            #            $hresult = $soap->headers[$header->name];
            #            $expect = $this->decodeSoapval($header);
            #            $ok = !$need_result || $this->compareResult($hresult ,$expect);
            #        }
            #        $header_result[$header->name]['ok'] = $ok;
            #        if (!$ok) $headers_ok = FALSE;
            #    }
            #}

            # we need to decode what we sent so we can compare!
            $sent_d = $this->decodeSoapval($sent);
            
            $soap_test->result['sent'] = $sent;
            $soap_test->result['return'] = $return;
            // compare the results with what we sent
            $ok = $this->compareResult($sent_d,$return, $sent->type);
            if (!$ok && $soap_test->expect) {
                $ok = $this->compareResult($soap_test->expect,$return);
            }
            
            if($ok){
                if (!$headers_ok) {
                    $fault = new SoapFault('HEADER','The returned result did not match what we expected to receive');
                    $soap_test->setResult(0,$fault->faultcode,
                                      $wire,
                                      $fault->faultstring,
                                      $fault
                                      );
                } else {
                    $soap_test->setResult(1,'OK',$wire);
                    $success = TRUE;
                }
            } else {
                $fault = new SoapFault('RESULT','The returned result did not match what we expected to receive');
                $soap_test->setResult(0,$fault->faultcode,
                                  $wire,
                                  $fault->faultstring,
                                  $fault
                                  );
            }
        } else {
            $fault = $soap->__getfault();
            if ($soap_test->expect_fault) {
                $ok = 1;
                $res = 'OK';
            } else {
                $ok = 0;
                $res =$fault->faultcode;
            }
            $soap_test->setResult($ok,$res, $wire,$fault->faultstring, $fault);
        }
        return $ok;
    }
    

    /**
    *  doTest
    *  run a single round of tests
    *
    * @access public
    */    
    function doTest() {
        global $soap_tests;
        // get endpoints for this test
        $this->getEndpoints($this->currentTest);
        #clear totals
        $this->totals = array();
        
        $i = 0;
        foreach($this->endpoints as $endpoint => $endpoint_info){
            
            // if we specify an endpoint, skip until we find it
            if ($this->specificEndpoint && $endpoint != $this->specificEndpoint) continue;
            if ($this->useWSDL && !$endpoint_info['endpointURL']) continue;
            
            $skipendpoint = FALSE;
            $this->totals['servers']++;
            #$endpoint_info['tests'] = array();
            
            if ($this->show) print "Processing $endpoint at {$endpoint_info['endpointURL']}<br>\n";
            
            foreach($soap_tests[$this->currentTest] as $soap_test) {
            //foreach(array_keys($method_params[$this->currentTest][$this->paramType]) as $method)
            
                // only run the type of test we're looking for (php or soapval)
                if ($soap_test->type != $this->paramType) continue;
                
                // if we haven't reached our startpoint, skip
                if ($this->startAt && $this->startAt != $endpoint_info['endpointName']) continue;
                $this->startAt = '';
            
                // if this is in our skip list, skip it
                if (in_array($endpoint, $this->skipEndpointList)) {
                    $skipendpoint = TRUE;
                    $skipfault = new SoapFault('SKIP','endpoint skipped');
                    $soap_test->setResult(0,$fault->faultcode, '',
                                  $skipfault->faultstring,
                                  $skipfault
                                  );
                    #$endpoint_info['tests'][] = &$soap_test;
                    #$soap_test->showTestResult($this->debug);
                    #$this->_saveResults($endpoint_info['id'], $soap_test->method_name);
                    $soap_test->result = NULL;
                    continue;
                }
                
                // if we're looking for a specific method, skip unless we have it
                if ($this->testMethod && !strstr($this->testMethod,$soap_test->test_name)) continue;
                if ($this->testMethod && $this->currentTest == 'GroupC') {
                    // we have to figure things out now
                    if (!preg_match('/(.*):(.*),(\d),(\d)/',$this->testMethod, $m)) continue;
                    
                    // is the header in the headers list?
                    $gotit = FALSE;
                    foreach ($soap_test->headers as $header) {
                        if (get_class($header) == 'soap_header') {
                            if ($header->name == $m[2]) {
                                $gotit = $header->attributes['SOAP-ENV:actor'] == ($m[3]?SOAP_TEST_ACTOR_NEXT:SOAP_TEST_ACTOR_OTHER);
                                $gotit = $gotit && $header->attributes['SOAP-ENV:mustUnderstand'] == $m[4];
                            }
                        } else {
                            if ($header[0] == $m[2]) {
                                $gotit = $gotit && $header[3] == ($m[3]?SOAP_TEST_ACTOR_NEXT:SOAP_TEST_ACTOR_OTHER);
                                $gotit = $gotit && $header[4] == $m[4];
                            }
                        }
                    }
                    if (!$gotit) continue;
                }
            
                // if we are skipping the rest of the tests (due to error) note a fault
                if ($skipendpoint) {
                    $soap_test->setResult(0,$fault->faultcode, '',
                                  $skipfault->faultstring,
                                  $skipfault
                                  );
                    #$endpoint_info['tests'][] = &$soap_test;
                    $this->totals['fail']++;
                } else {
                    // run the endpoint test
                    if ($this->doEndpointMethod($endpoint_info, $soap_test)) {
                        $this->totals['success']++;
                    } else {
                        $skipendpoint = $soap_test->result['fault']->faultcode=='HTTP'
                            && strstr($soap_test->result['fault']->faultstring,'Connect Error');
                        $skipfault = $soap_test->result['fault'];
                        $this->totals['fail']++;
                    }
                    #$endpoint_info['tests'][] = &$soap_test;
                }
                $soap_test->showTestResult($this->debug);
                $this->_saveResults($endpoint_info['id'], $soap_test);
                $soap_test->result = NULL;
                $this->totals['calls']++;
            }
            if ($this->numservers && ++$i >= $this->numservers) break;
        }
    }
    
    function doGroupTests() {
        $dowsdl = array(0,1);
        foreach($dowsdl as $usewsdl) {
            $this->useWSDL = $usewsdl;
            foreach($this->paramTypes as $ptype) {
                // skip a pointless test
                if ($usewsdl && $ptype == 'soapval') break;
                $this->paramType = $ptype;
                $this->doTest();
            }
        }
    }
    
    /**
    *  doTests
    *  go all out.  This takes time.
    *
    * @access public
    */    
    function doTests() {
        // the mother of all interop tests
        $dowsdl = array(0,1);
        foreach($this->tests as $test) {
            $this->currentTest = $test;
            foreach($dowsdl as $usewsdl) {
                $this->useWSDL = $usewsdl;
                foreach($this->paramTypes as $ptype) {
                    // skip a pointless test
                    if ($usewsdl && $ptype == 'soapval') break;
                    $this->paramType = $ptype;
                    $this->doTest();
                }
            }
        }
    }
    
    // ***********************************************************
    // output functions
    
    /**
    *  getResults
    * retreive results from the database, stuff them into the endpoint array
    *
    * @access private
    */
    function getMethodList($test = 'base') {
        // retreive the results and put them into the endpoint info
        $sql = "select distinct(function) from results where class='$test' order by function";
        $results = $this->dbc->getAll($sql);
        $ar = array();
        foreach($results as $result) {
            $ar[] = $result[0];
        }
        return $ar;
    }
    
    function outputTable()
    {
        $methods = $this->getMethodList($this->currentTest);
        if (!$methods) return;
        $this->getResults($this->currentTest,$this->paramType,$this->useWSDL);
        
        echo "<b>Testing $this->currentTest ";
        if ($this->useWSDL) echo "using WSDL ";
        else echo "using Direct calls ";
        echo "with $this->paramType values</b><br>\n";
        
        // calculate totals for this table
        $this->totals['success'] = 0;
        $this->totals['fail'] = 0;
        $this->totals['servers'] = 0; #count($this->endpoints);
        foreach ($this->endpoints as $endpoint => $endpoint_info) {
            if (count($endpoint_info['methods']) > 0) {
                $this->totals['servers']++;
                foreach ($methods as $method) {
                    $r = $endpoint_info['methods'][$method]['result'];
                    if ($r == 'OK') $this->totals['success']++;
                    else $this->totals['fail']++;
                }
            } else {
                unset($this->endpoints[$endpoint]);
            }
        }
        $this->totals['calls'] = count($methods) * $this->totals['servers'];

        if ($this->totals['fail'] == $this->totals['calls']) {
            // assume tests have not run, skip outputing table
            print "No Data Available<br>\n";
            return;
        }
        
        echo "\n\n<b>Servers: {$this->totals['servers']} Calls: {$this->totals['calls']} Success: {$this->totals['success']} Fail: {$this->totals['fail']}</b><br>\n";
       
        echo "<table border=\"1\" cellspacing=\"0\" cellpadding=\"2\">\n";
        echo "<tr><td class=\"BLANK\">Endpoint</td>\n";
        foreach ($methods as $method) {
            $info = split(':', $method);
            echo "<td class='BLANK' valign='top'>";
            foreach ($info as $m) {
                $hi = split(',',$m);
                echo '<b>'.$hi[0]."</b><br>\n";
                if (count($hi) > 1) {
                    echo "&nbsp;&nbsp;Actor=".($hi[1]?'Target':'Not Target')."<br>\n";
                    echo "&nbsp;&nbsp;MustUnderstand=$hi[2]<br>\n";
                }
            }
            echo "</td>\n";
        }
        echo "</tr>\n";
        $faults = array();
        $fi = 0;
        foreach ($this->endpoints as $endpoint => $endpoint_info) {
            if (array_key_exists('wsdlURL',$endpoint_info)) {
                echo "<tr><td class=\"BLANK\"><a href=\"{$endpoint_info['wsdlURL']}\">$endpoint</a></td>\n";
            } else {
                echo "<tr><td class=\"BLANK\">$endpoint</td>\n";
            }
            foreach ($methods as $method) {
                $id = $endpoint_info['methods'][$method]['id'];
                $r = $endpoint_info['methods'][$method]['result'];
                $e = $endpoint_info['methods'][$method]['error'];
                if ($e) {
                    $faults[$fi++] = $e;
                }
                if ($r) {
                    echo "<td class='$r'><a href='$PHP_SELF?wire=$id'>$r</a></td>\n";
                } else {
                    echo "<td class='untested'>untested</td>\n";
                }
            }
            echo "</tr>\n";
        }
        echo "</table><br>\n";
        if ($this->showFaults && count($faults) > 0) {
            echo "<b>ERROR Details:</b><br>\n<ul>\n";
            # output more error detail
            foreach ($faults as $fault) {
                echo '<li>'.HTMLSpecialChars($fault)."</li>\n";
            }
        }
        echo "</ul><br><br>\n";
    }
    
    function outputTables() {
        // the mother of all interop tests
        $dowsdl = array(0,1);
        foreach($this->tests as $test) {
            $this->currentTest = $test;
            foreach($dowsdl as $usewsdl) {
                $this->useWSDL = $usewsdl;
                foreach($this->paramTypes as $ptype) {
                    // skip a pointless test
                    if ($usewsdl && $ptype == 'soapval') break;
                    $this->paramType = $ptype;
                    $this->outputTable();
                }
            }
        }
    }
    
    function showWire($id) {
        $results = $this->dbc->getAll("select * from results where id=$id",NULL, DB_FETCHMODE_ASSOC );
        #$wire = preg_replace("/>/",">\n",$results[0]['wire']);
        $wire = $results[0]['wire'];
        echo "<pre>\n".HTMLSpecialChars($wire)."</pre>\n";
    }

}

?>