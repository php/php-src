<?php
require_once 'XML/RPC/Server.php';

class PEAR_test_mock_pearweb {
    var $_config;
    
    function addHtmlConfig($address, $contents)
    {
        $this->_config['html'][$address] = $contents;
    }
    
    function addXmlrpcConfig($method, $params, $return)
    {
        $val = XML_RPC_encode($return);
        $ser = new XML_RPC_Response($val);
        $this->_config['xmlrpc'][$method][serialize($params)] = $ser->serialize();
    }
    
    function receiveHttp($address)
    {
        if (!isset($this->_config) || !is_array($this->_config)) {
            return $this->do404($address);
        }
        if (!isset($this->_config[$address])) {
            return $this->do404($address);
        }
        if (isset($this->_config['html'][$address])) {
            return $this->do200() . $this->_config['html'][$address];
        }
    }
    
    function receiveXmlrpc($postpayload)
    {
        $info = $this->parseRequest($postpayload);
        if (!isset($this->_config['xmlrpc'][$info['method']])) {
            return $this->doXmlrpcFault($info);
        }
        if (!isset($this->_config['xmlrpc'][serialize($info['params'])])) {
            var_dump($info['param']);
            die("Error - parameters not configured properly for $info[method]");
        }
        return $this->do200() . $this->_config['xmlrpc'][$info['method']][serialize($info['params'])];
    }
    
    function doXmlrpcFault($info)
    {
        $r = new XML_RPC_Response(0, 1, 'Unknown method');
        return $this->do200() . $r->serialize();
    }
    
    function do200()
    {
        return 'HTTP/1.1 200 ';
    }
    
    function do404($address)
    {
        return 'HTTP/1.1 404 ' . $address . ' Is not valid';
    }
    
    /**
     * Parse an xmlrpc request
     * @param string fake HTTP_RAW_POST_DATA
     * @return string|array serialized fault string, or array containing method name and parameters
     */
    function parseRequest($data)
    {
        // copied from XML_RPC_Server
        global $XML_RPC_xh;
        global $XML_RPC_err, $XML_RPC_str, $XML_RPC_errxml,
            $XML_RPC_defencoding, $XML_RPC_Server_dmap;

        $parser = xml_parser_create($XML_RPC_defencoding);

        $XML_RPC_xh[$parser] = array();
        $XML_RPC_xh[$parser]['st'] = "";
        $XML_RPC_xh[$parser]['cm'] = 0;
        $XML_RPC_xh[$parser]['isf'] = 0;
        $XML_RPC_xh[$parser]['params'] = array();
        $XML_RPC_xh[$parser]['method'] = "";

        $plist = '';

        // decompose incoming XML into request structure

        xml_parser_set_option($parser, XML_OPTION_CASE_FOLDING, true);
        xml_set_element_handler($parser, "XML_RPC_se", "XML_RPC_ee");
        xml_set_character_data_handler($parser, "XML_RPC_cd");
        xml_set_default_handler($parser, "XML_RPC_dh");
        if (!xml_parse($parser, $data, 1)) {
            // return XML error as a faultCode
            $r = new XML_RPC_Response(0,
                                      $XML_RPC_errxml+xml_get_error_code($parser),
                                      sprintf("XML error: %s at line %d",
                                              xml_error_string(xml_get_error_code($parser)),
                                              xml_get_current_line_number($parser)));
            xml_parser_free($parser);
            return $r->serialize();
        } else {
            xml_parser_free($parser);
            $params = array();
            // now add parameters in
            for ($i = 0; $i < sizeof($XML_RPC_xh[$parser]['params']); $i++) {
                // print "<!-- " . $XML_RPC_xh[$parser]['params'][$i]. "-->\n";
                $plist .= "$i - " . $XML_RPC_xh[$parser]['params'][$i] . " \n";
                eval('$val = ' . $XML_RPC_xh[$parser]['params'][$i] . ";");
                $param = $val->scalarval();
                $param = PEAR_test_mock_pearweb::_convertScalar($param);
                $params[] = $param;
            }
            return array('method' => $XML_RPC_xh[$parser]['method'], 'params' => $params);
        }
    }
    
    /**
     * Converts the mishmash returned from XML_RPC parsing into a regular PHP value,
     * handling nested arrays gracefully.
     * @param mixed
     * @return mixed
     */
    function _convertScalar($val)
    {
        if (is_a($val, 'XML_RPC_Value')) {
            $val = $val->scalarval();
        }
        if (!is_array($val)) {
            return $val;
        }
        $newval = array();
        foreach ($val as $i => $contents)
        {
            $newval[$i] = PEAR_test_mock_pearweb::_convertScalar($contents);
        }
        return $newval;
    }
}
?>