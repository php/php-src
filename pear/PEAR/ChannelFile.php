<?php
/**
 * PEAR_ChannelFile, the channel handling class
 *
 * PHP versions 4 and 5
 *
 * LICENSE: This source file is subject to version 3.0 of the PHP license
 * that is available through the world-wide-web at the following URI:
 * http://www.php.net/license/3_0.txt.  If you did not receive a copy of
 * the PHP License and are unable to obtain it through the web, please
 * send a note to license@php.net so we can mail you a copy immediately.
 *
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 1.4.0a1
 */

/**
 * Needed for error handling
 */
require_once 'PEAR/ErrorStack.php';
require_once 'PEAR/XMLParser.php';
require_once 'PEAR/Common.php';

/**
 * Error code if the channel.xml <channel> tag does not contain a valid version
 */
define('PEAR_CHANNELFILE_ERROR_NO_VERSION', 1);
/**
 * Error code if the channel.xml <channel> tag version is not supported (version 1.0 is the only supported version,
 * currently
 */
define('PEAR_CHANNELFILE_ERROR_INVALID_VERSION', 2);

/**
 * Error code if parsing is attempted with no xml extension
 */
define('PEAR_CHANNELFILE_ERROR_NO_XML_EXT', 3);

/**
 * Error code if creating the xml parser resource fails
 */
define('PEAR_CHANNELFILE_ERROR_CANT_MAKE_PARSER', 4);

/**
 * Error code used for all sax xml parsing errors
 */
define('PEAR_CHANNELFILE_ERROR_PARSER_ERROR', 5);

/**#@+
 * Validation errors
 */
/**
 * Error code when channel name is missing
 */
define('PEAR_CHANNELFILE_ERROR_NO_NAME', 6);
/**
 * Error code when channel name is invalid
 */
define('PEAR_CHANNELFILE_ERROR_INVALID_NAME', 7);
/**
 * Error code when channel summary is missing
 */
define('PEAR_CHANNELFILE_ERROR_NO_SUMMARY', 8);
/**
 * Error code when channel summary is multi-line
 */
define('PEAR_CHANNELFILE_ERROR_MULTILINE_SUMMARY', 9);
/**
 * Error code when channel server is missing for xmlrpc or soap protocol
 */
define('PEAR_CHANNELFILE_ERROR_NO_HOST', 10);
/**
 * Error code when channel server is invalid for xmlrpc or soap protocol
 */
define('PEAR_CHANNELFILE_ERROR_INVALID_HOST', 11);
/**
 * Error code when a mirror name is invalid
 */
define('PEAR_CHANNELFILE_ERROR_INVALID_MIRROR', 21);
/**
 * Error code when a mirror type is invalid
 */
define('PEAR_CHANNELFILE_ERROR_INVALID_MIRRORTYPE', 22);
/**
 * Error code when an attempt is made to generate xml, but the parsed content is invalid
 */
define('PEAR_CHANNELFILE_ERROR_INVALID', 23);
/**
 * Error code when an empty package name validate regex is passed in
 */
define('PEAR_CHANNELFILE_ERROR_EMPTY_REGEX', 24);
/**
 * Error code when a <function> tag has no version
 */
define('PEAR_CHANNELFILE_ERROR_NO_FUNCTIONVERSION', 25);
/**
 * Error code when a <function> tag has no name
 */
define('PEAR_CHANNELFILE_ERROR_NO_FUNCTIONNAME', 26);
/**
 * Error code when a <validatepackage> tag has no name
 */
define('PEAR_CHANNELFILE_ERROR_NOVALIDATE_NAME', 27);
/**
 * Error code when a <validatepackage> tag has no version attribute
 */
define('PEAR_CHANNELFILE_ERROR_NOVALIDATE_VERSION', 28);
/**
 * Error code when a mirror does not exist but is called for in one of the set*
 * methods.
 */
define('PEAR_CHANNELFILE_ERROR_MIRROR_NOT_FOUND', 32);
/**
 * Error code when a server port is not numeric
 */
define('PEAR_CHANNELFILE_ERROR_INVALID_PORT', 33);
/**
 * Error code when <static> contains no version attribute
 */
define('PEAR_CHANNELFILE_ERROR_NO_STATICVERSION', 34);
/**
 * Error code when <baseurl> contains no type attribute in a <rest> protocol definition
 */
define('PEAR_CHANNELFILE_ERROR_NOBASEURLTYPE', 35);
/** 
 * Error code when a mirror is defined and the channel.xml represents the __uri pseudo-channel
 */
define('PEAR_CHANNELFILE_URI_CANT_MIRROR', 36);
/** 
 * Error code when ssl attribute is present and is not "yes"
 */
define('PEAR_CHANNELFILE_ERROR_INVALID_SSL', 37);
/**#@-*/

/**
 * Mirror types allowed.  Currently only internet servers are recognized.
 */
$GLOBALS['_PEAR_CHANNELS_MIRROR_TYPES'] =  array('server');


/**
 * The Channel handling class
 *
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_ChannelFile {
    /**
     * @access private
     * @var PEAR_ErrorStack
     * @access private
     */
    var $_stack;
    
    /**
     * Supported channel.xml versions, for parsing
     * @var array
     * @access private
     */
    var $_supportedVersions = array('1.0');

    /**
     * Parsed channel information
     * @var array
     * @access private
     */
    var $_channelInfo;

    /**
     * index into the subchannels array, used for parsing xml
     * @var int
     * @access private
     */
    var $_subchannelIndex;

    /**
     * index into the mirrors array, used for parsing xml
     * @var int
     * @access private
     */
    var $_mirrorIndex;
    
    /**
     * Flag used to determine the validity of parsed content
     * @var boolean
     * @access private
     */
    var $_isValid = false;

    function PEAR_ChannelFile()
    {
        $this->_stack = &new PEAR_ErrorStack('PEAR_ChannelFile');
        $this->_stack->setErrorMessageTemplate($this->_getErrorMessage());
        $this->_isValid = false;
    }
    
    /**
     * @return array
     * @access protected
     */
    function _getErrorMessage()
    {
        return
            array(
                PEAR_CHANNELFILE_ERROR_INVALID_VERSION =>
                    'While parsing channel.xml, an invalid version number "%version% was passed in, expecting one of %versions%',
                PEAR_CHANNELFILE_ERROR_NO_VERSION =>
                    'No version number found in <channel> tag',
                PEAR_CHANNELFILE_ERROR_NO_XML_EXT =>
                    '%error%',
                PEAR_CHANNELFILE_ERROR_CANT_MAKE_PARSER =>
                    'Unable to create XML parser',
                PEAR_CHANNELFILE_ERROR_PARSER_ERROR =>
                    '%error%',
                PEAR_CHANNELFILE_ERROR_NO_NAME =>
                    'Missing channel name',
                PEAR_CHANNELFILE_ERROR_INVALID_NAME =>
                    'Invalid channel %tag% "%name%"',
                PEAR_CHANNELFILE_ERROR_NO_SUMMARY =>
                    'Missing channel summary',
                PEAR_CHANNELFILE_ERROR_MULTILINE_SUMMARY =>
                    'Channel summary should be on one line, but is multi-line',
                PEAR_CHANNELFILE_ERROR_NO_HOST =>
                    'Missing channel server for %type% server',
                PEAR_CHANNELFILE_ERROR_INVALID_HOST =>
                    'Server name "%server%" is invalid for %type% server',
                PEAR_CHANNELFILE_ERROR_INVALID_MIRROR =>
                    'Invalid mirror name "%name%", mirror type %type%',
                PEAR_CHANNELFILE_ERROR_INVALID_MIRRORTYPE =>
                    'Invalid mirror type "%type%"',
                PEAR_CHANNELFILE_ERROR_INVALID =>
                    'Cannot generate xml, contents are invalid',
                PEAR_CHANNELFILE_ERROR_EMPTY_REGEX =>
                    'packagenameregex cannot be empty',
                PEAR_CHANNELFILE_ERROR_NO_FUNCTIONVERSION =>
                    '%parent% %protocol% function has no version',
                PEAR_CHANNELFILE_ERROR_NO_FUNCTIONNAME =>
                    '%parent% %protocol% function has no name',
                PEAR_CHANNELFILE_ERROR_NOBASEURLTYPE =>
                    '%parent% rest baseurl has no type',
                PEAR_CHANNELFILE_ERROR_NOVALIDATE_NAME =>
                    'Validation package has no name in <validatepackage> tag',
                PEAR_CHANNELFILE_ERROR_NOVALIDATE_VERSION =>
                    'Validation package "%package%" has no version',
                PEAR_CHANNELFILE_ERROR_MIRROR_NOT_FOUND =>
                    'Mirror "%mirror%" does not exist',
                PEAR_CHANNELFILE_ERROR_INVALID_PORT =>
                    'Port "%port%" must be numeric',
                PEAR_CHANNELFILE_ERROR_NO_STATICVERSION =>
                    '<static> tag must contain version attribute',
                PEAR_CHANNELFILE_URI_CANT_MIRROR =>
                    'The __uri pseudo-channel cannot have mirrors',
                PEAR_CHANNELFILE_ERROR_INVALID_SSL =>
                    '%server% has invalid ssl attribute "%ssl%" can only be yes or not present',
            );
    }

    /**
     * @param string contents of package.xml file
     * @return bool success of parsing
     */
    function fromXmlString($data)
    {
        if (preg_match('/<channel\s+version="([0-9]+\.[0-9]+)"/', $data, $channelversion)) {
            if (!in_array($channelversion[1], $this->_supportedVersions)) {
                $this->_stack->push(PEAR_CHANNELFILE_ERROR_INVALID_VERSION, 'error',
                    array('version' => $channelversion[1]));
                return false;
            }
            $parser = new PEAR_XMLParser;
            $result = $parser->parse($data);
            if ($result !== true) {
                if ($result->getCode() == 1) {
                    $this->_stack->push(PEAR_CHANNELFILE_ERROR_NO_XML_EXT, 'error',
                        array('error' => $error));
                } else {
                    $this->_stack->push(PEAR_CHANNELFILE_ERROR_CANT_MAKE_PARSER, 'error');
                }
                return false;
            }
            $this->_channelInfo = $parser->getData();
            return true;
        } else {
            $this->_stack->push(PEAR_CHANNELFILE_ERROR_NO_VERSION, 'error', array('xml' => $data));
            return false;
        }
    }
    
    /**
     * @return array
     */
    function toArray()
    {
        if (!$this->_isValid && !$this->validate()) {
            return false;
        }
        return $this->_channelInfo;
    }
    
    /**
     * @param array
     * @static
     * @return PEAR_ChannelFile|false false if invalid
     */
    function &fromArray($data, $compatibility = false, $stackClass = 'PEAR_ErrorStack')
    {
        $a = new PEAR_ChannelFile($compatibility, $stackClass);
        $a->_fromArray($data);
        if (!$a->validate()) {
            $a = false;
            return $a;
        }
        return $a;
    }

    /**
     * Unlike {@link fromArray()} this does not do any validation
     * @param array
     * @static
     * @return PEAR_ChannelFile
     */
    function &fromArrayWithErrors($data, $compatibility = false,
                                  $stackClass = 'PEAR_ErrorStack')
    {
        $a = new PEAR_ChannelFile($compatibility, $stackClass);
        $a->_fromArray($data);
        return $a;
    }
    
    /**
     * @param array
     * @access private
     */
    function _fromArray($data)
    {
        $this->_channelInfo = $data;
    }
    
    /**
     * Wrapper to {@link PEAR_ErrorStack::getErrors()}
     * @param boolean determines whether to purge the error stack after retrieving
     * @return array
     */
    function getErrors($purge = false)
    {
        return $this->_stack->getErrors($purge);
    }

    /**
     * Unindent given string (?)
     *
     * @param string $str The string that has to be unindented.
     * @return string
     * @access private
     */
    function _unIndent($str)
    {
        // remove leading newlines
        $str = preg_replace('/^[\r\n]+/', '', $str);
        // find whitespace at the beginning of the first line
        $indent_len = strspn($str, " \t");
        $indent = substr($str, 0, $indent_len);
        $data = '';
        // remove the same amount of whitespace from following lines
        foreach (explode("\n", $str) as $line) {
            if (substr($line, 0, $indent_len) == $indent) {
                $data .= substr($line, $indent_len) . "\n";
            }
        }
        return $data;
    }

    /**
     * Parse a channel.xml file.  Expects the name of
     * a channel xml file as input.
     *
     * @param string  $descfile  name of channel xml file
     * @return bool success of parsing
     */
    function fromXmlFile($descfile)
    {
        if (!@is_file($descfile) || !is_readable($descfile) ||
             (!$fp = @fopen($descfile, 'r'))) {
            require_once 'PEAR.php';
            return PEAR::raiseError("Unable to open $descfile");
        }

        // read the whole thing so we only get one cdata callback
        // for each block of cdata
        if (function_exists('file_get_contents')) {
            fclose($fp);
            $data = file_get_contents($descfile);
        } else {
            $data = fread($fp, filesize($descfile));
            fclose($fp);
        }
        return $this->fromXmlString($data);
    }

    /**
     * Parse channel information from different sources
     *
     * This method is able to extract information about a channel
     * from an .xml file or a string
     *
     * @access public
     * @param  string Filename of the source or the source itself
     * @return bool
     */
    function fromAny($info)
    {
        if (is_string($info) && file_exists($info) && strlen($info) < 255) {
            $tmp = substr($info, -4);
            if ($tmp == '.xml') {
                $info = $this->fromXmlFile($info);
            } else {
                $fp = fopen($info, "r");
                $test = fread($fp, 5);
                fclose($fp);
                if ($test == "<?xml") {
                    $info = $this->fromXmlFile($info);
                }
            }
            if (PEAR::isError($info)) {
                require_once 'PEAR.php';
                return PEAR::raiseError($info);
            }
        }
        if (is_string($info)) {
            $info = $this->fromXmlString($info);
        }
        return $info;
    }

    /**
     * Return an XML document based on previous parsing and modifications
     *
     * @return string XML data
     *
     * @access public
     */
    function toXml()
    {
        if (!$this->_isValid && !$this->validate()) {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_INVALID);
            return false;
        }
        if (!isset($this->_channelInfo['attribs']['version'])) {
            $this->_channelInfo['attribs']['version'] = '1.0';
        }
        $channelInfo = $this->_channelInfo;
        $ret = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n";
        $ret .= "<channel version=\"" .
            $channelInfo['attribs']['version'] . "\" xmlns=\"http://pear.php.net/channel-1.0\"
  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"
  xsi:schemaLocation=\"http://pear.php.net/dtd/channel-"
            . $channelInfo['attribs']['version'] . " http://pear.php.net/dtd/channel-" .
            $channelInfo['attribs']['version'] . ".xsd\">
 <name>$channelInfo[name]</name>
 <summary>" . htmlspecialchars($channelInfo['summary'])."</summary>
";
        if (isset($channelInfo['suggestedalias'])) {
            $ret .= ' <suggestedalias>' . $channelInfo['suggestedalias'] . "</suggestedalias>\n";
        }
        if (isset($channelInfo['validatepackage'])) {
            $ret .= ' <validatepackage version="' .
                $channelInfo['validatepackage']['attribs']['version']. '">' .
                htmlspecialchars($channelInfo['validatepackage']['_content']) .
                "</validatepackage>\n";
        }
        $ret .= " <servers>\n";
        $ret .= '  <primary';
        if (isset($channelInfo['servers']['primary']['attribs']['ssl'])) {
            $ret .= ' ssl="' . $channelInfo['servers']['primary']['attribs']['ssl'] . '"';
        }
        if (isset($channelInfo['servers']['primary']['attribs']['port'])) {
            $ret .= ' port="' . $channelInfo['servers']['primary']['attribs']['port'] . '"';
        }
        $ret .= ">\n";
        if (isset($channelInfo['servers']['primary']['xmlrpc'])) {
            $ret .= $this->_makeXmlrpcXml($channelInfo['servers']['primary']['xmlrpc'], '   ');
        }
        if (isset($channelInfo['servers']['primary']['rest'])) {
            $ret .= $this->_makeRestXml($channelInfo['servers']['primary']['rest'], '   ');
        }
        if (isset($channelInfo['servers']['primary']['soap'])) {
            $ret .= $this->_makeSoapXml($channelInfo['servers']['primary']['soap'], '   ');
        }
        $ret .= "  </primary>\n";
        if (isset($channelInfo['servers']['mirror'])) {
            $ret .= $this->_makeMirrorsXml($channelInfo);
        }
        $ret .= " </servers>\n";
        $ret .= "</channel>";
        return str_replace("\r", "\n", str_replace("\r\n", "\n", $ret));
    }

    /**
     * Generate the <xmlrpc> tag
     * @access private
     */
    function _makeXmlrpcXml($info, $indent)
    {
        $ret = $indent . "<xmlrpc";
        if (isset($info['attribs']['path'])) {
            $ret .= ' path="' . htmlspecialchars($info['attribs']['path']) . '"';
        }
        $ret .= ">\n";
        $ret .= $this->_makeFunctionsXml($info['function'], "$indent ");
        $ret .= $indent . "</xmlrpc>\n";
        return $ret;
    }

    /**
     * Generate the <soap> tag
     * @access private
     */
    function _makeSoapXml($info, $indent)
    {
        $ret = $indent . "<soap";
        if (isset($info['attribs']['path'])) {
            $ret .= ' path="' . htmlspecialchars($info['attribs']['path']) . '"';
        }
        $ret .= ">\n";
        $ret .= $this->_makeFunctionsXml($info['function'], "$indent ");
        $ret .= $indent . "</soap>\n";
        return $ret;
    }

    /**
     * Generate the <rest> tag
     * @access private
     */
    function _makeRestXml($info, $indent)
    {
        $ret = $indent . "<rest>\n";
        if (!isset($info['baseurl'][0])) {
            $info['baseurl'] = array($info['baseurl']);
        }
        foreach ($info['baseurl'] as $url) {
            $ret .= "$indent <baseurl type=\"" . $url['attribs']['type'] . "\"";
            $ret .= ">" . $url['_content'] . "</baseurl>\n";
        }
        $ret .= $indent . "</rest>\n";
        return $ret;
    }

    /**
     * Generate the <mirrors> tag
     * @access private
     */
    function _makeMirrorsXml($channelInfo)
    {
        $ret = "";
        if (!isset($channelInfo['servers']['mirror'][0])) {
            $channelInfo['servers']['mirror'] = array($channelInfo['servers']['mirror']);
        }
        foreach ($channelInfo['servers']['mirror'] as $mirror) {
            $ret .= '  <mirror host="' . $mirror['attribs']['host'] . '"';
            if (isset($mirror['attribs']['port'])) {
                $ret .= ' port="' . $mirror['attribs']['port'] . '"';
            }
            if (isset($mirror['attribs']['ssl'])) {
                $ret .= ' ssl="' . $mirror['attribs']['ssl'] . '"';
            }
            $ret .= ">\n";
            if (isset($mirror['xmlrpc']) || isset($mirror['soap'])) {
                if (isset($mirror['xmlrpc'])) {
                    $ret .= $this->_makeXmlrpcXml($mirror['xmlrpc'], '   ');
                }
                if (isset($mirror['rest'])) {
                    $ret .= $this->_makeRestXml($mirror['rest'], '   ');
                }
                if (isset($mirror['soap'])) {
                    $ret .= $this->_makeSoapXml($mirror['soap'], '   ');
                }
                $ret .= "  </mirror>\n";
            } else {
                $ret .= "/>\n";
            }
        }
        return $ret;
    }

    /**
     * Generate the <functions> tag
     * @access private
     */
    function _makeFunctionsXml($functions, $indent, $rest = false)
    {
        $ret = '';
        if (!isset($functions[0])) {
            $functions = array($functions);
        }
        foreach ($functions as $function) {
            $ret .= "$indent<function version=\"" . $function['attribs']['version'] . "\"";
            if ($rest) {
                $ret .= ' uri="' . $function['attribs']['uri'] . '"';
            }
            $ret .= ">" . $function['_content'] . "</function>\n";
        }
        return $ret;
    }

    /**
     * Validation error.  Also marks the object contents as invalid
     * @param error code
     * @param array error information
     * @access private
     */
    function _validateError($code, $params = array())
    {
        $this->_stack->push($code, 'error', $params);
        $this->_isValid = false;
    }

    /**
     * Validation warning.  Does not mark the object contents invalid.
     * @param error code
     * @param array error information
     * @access private
     */
    function _validateWarning($code, $params = array())
    {
        $this->_stack->push($code, 'warning', $params);
    }

    /**
     * Validate parsed file.
     *
     * @access public
     * @return boolean
     */
    function validate()
    {
        $this->_isValid = true;
        $info = $this->_channelInfo;
        if (empty($info['name'])) {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_NO_NAME);
        } elseif (!$this->validChannelServer($info['name'])) {
            if ($info['name'] != '__uri') {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_INVALID_NAME, array('tag' => 'name',
                    'name' => $info['name']));
            }
        }
        if (empty($info['summary'])) {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_NO_SUMMARY);
        } elseif (strpos(trim($info['summary']), "\n") !== false) {
            $this->_validateWarning(PEAR_CHANNELFILE_ERROR_MULTILINE_SUMMARY,
                array('summary' => $info['summary']));
        }
        if (isset($info['suggestedalias'])) {
            if (!$this->validChannelServer($info['suggestedalias'])) {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_INVALID_NAME,
                    array('tag' => 'suggestedalias', 'name' =>$info['suggestedalias']));
            }
        }
        if (isset($info['localalias'])) {
            if (!$this->validChannelServer($info['localalias'])) {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_INVALID_NAME,
                    array('tag' => 'localalias', 'name' =>$info['localalias']));
            }
        }
        if (isset($info['validatepackage'])) {
            if (!isset($info['validatepackage']['_content'])) {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_NOVALIDATE_NAME);
            }
            if (!isset($info['validatepackage']['attribs']['version'])) {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_NOVALIDATE_VERSION,
                    array('package' => @$info['validatepackage']['_content']));
            }
        }
        if (isset($info['servers']['primary']['attribs']['port']) &&
              !is_numeric($info['servers']['primary']['attribs']['port'])) {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_INVALID_PORT,
                array('port' => $info['servers']['primary']['attribs']['port']));
        }
        if (isset($info['servers']['primary']['attribs']['ssl']) &&
              $info['servers']['primary']['attribs']['ssl'] != 'yes') {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_INVALID_SSL,
                array('ssl' => $info['servers']['primary']['attribs']['ssl'],
                    'server' => $info['name']));
        }

        if (isset($info['servers']['primary']['xmlrpc']) &&
              isset($info['servers']['primary']['xmlrpc']['function'])) {
            $this->_validateFunctions('xmlrpc', $info['servers']['primary']['xmlrpc']['function']);
        }
        if (isset($info['servers']['primary']['soap']) &&
              isset($info['servers']['primary']['soap']['function'])) {
            $this->_validateFunctions('soap', $info['servers']['primary']['soap']['function']);
        }
        if (isset($info['servers']['primary']['rest']) &&
              isset($info['servers']['primary']['rest']['baseurl'])) {
            $this->_validateFunctions('rest', $info['servers']['primary']['rest']['baseurl']);
        }
        if (isset($info['servers']['mirror'])) {
            if ($this->_channelInfo['name'] == '__uri') {
                $this->_validateError(PEAR_CHANNELFILE_URI_CANT_MIRROR);
            }
            if (!isset($info['servers']['mirror'][0])) {
                $info['servers']['mirror'] = array($info['servers']['mirror']);
            }
            $i = 0;
            foreach ($info['servers']['mirror'] as $mirror) {
                if (!isset($mirror['attribs']['host'])) {
                    $this->_validateError(PEAR_CHANNELFILE_ERROR_NO_HOST,
                      array('type' => 'mirror'));
                } elseif (!$this->validChannelServer($mirror['attribs']['host'])) {
                    $this->_validateError(PEAR_CHANNELFILE_ERROR_INVALID_HOST,
                        array('server' => $mirror['attribs']['host'], 'type' => 'mirror'));
                }
                if (isset($mirror['attribs']['ssl']) && $mirror['attribs']['ssl'] != 'yes') {
                    $this->_validateError(PEAR_CHANNELFILE_ERROR_INVALID_SSL,
                        array('ssl' => $info['ssl'], 'server' => $mirror['attribs']['host']));
                }
                if (isset($mirror['xmlrpc'])) {
                    $this->_validateFunctions('xmlrpc',
                        $mirror['xmlrpc']['function'], $mirror['attribs']['host']);
                }
                if (isset($mirror['soap'])) {
                    $this->_validateFunctions('soap', $mirror['soap']['function'],
                        $mirror['attribs']['host']);
                }
                if (isset($mirror['rest'])) {
                    $this->_validateFunctions('rest', $mirror['rest']['baseurl'],
                        $mirror['attribs']['host']);
                }
            }
        }
        return $this->_isValid;
    }

    /**
     * @param string xmlrpc or soap - protocol name this function applies to
     * @param array the functions
     * @param string the name of the parent element (mirror name, for instance)
     */
    function _validateFunctions($protocol, $functions, $parent = '')
    {
        if (!isset($functions[0])) {
            $functions = array($functions);
        }
        foreach ($functions as $function) {
            if (!isset($function['_content']) || empty($function['_content'])) {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_NO_FUNCTIONNAME,
                    array('parent' => $parent, 'protocol' => $protocol));
            }
            if ($protocol == 'rest') {
                if (!isset($function['attribs']['type']) ||
                      empty($function['attribs']['type'])) {
                    $this->_validateError(PEAR_CHANNELFILE_ERROR_NO_BASEURLTYPE,
                        array('parent' => $parent, 'protocol' => $protocol));
                }
            } else {
                if (!isset($function['attribs']['version']) ||
                      empty($function['attribs']['version'])) {
                    $this->_validateError(PEAR_CHANNELFILE_ERROR_NO_FUNCTIONVERSION,
                        array('parent' => $parent, 'protocol' => $protocol));
                }
            }
        }
    }

    /**
     * Test whether a string contains a valid channel server.
     * @param string $ver the package version to test
     * @return bool
     */
    function validChannelServer($server)
    {
        if ($server == '__uri') {
            return true;
        }
        return (bool) preg_match(PEAR_CHANNELS_SERVER_PREG, $server);
    }

    /**
     * @return string|false
     */
    function getName()
    {
        if (isset($this->_channelInfo['name'])) {
            return $this->_channelInfo['name'];
        } else {
            return false;
        }
    }

    /**
     * @return string|false
     */
    function getServer()
    {
        if (isset($this->_channelInfo['name'])) {
            return $this->_channelInfo['name'];
        } else {
            return false;
        }
    }

    /**
     * @return int|80 port number to connect to
     */
    function getPort($mirror = false)
    {
        if ($mirror) {
            if ($mir = $this->getMirror($mirror)) {
                if (isset($mir['attribs']['port'])) {
                    return $mir['attribs']['port'];
                } else {
                    if ($this->getSSL($mirror)) {
                        return 443;
                    }
                    return 80;
                }
            }
            return false;
        }
        if (isset($this->_channelInfo['servers']['primary']['attribs']['port'])) {
            return $this->_channelInfo['servers']['primary']['attribs']['port'];
        }
        if ($this->getSSL()) {
            return 443;
        }
        return 80;
    }

    /**
     * @return bool Determines whether secure sockets layer (SSL) is used to connect to this channel
     */
    function getSSL($mirror = false)
    {
        if ($mirror) {
            if ($mir = $this->getMirror($mirror)) {
                if (isset($mir['attribs']['ssl'])) {
                    return true;
                } else {
                    return false;
                }
            }
            return false;
        }
        if (isset($this->_channelInfo['servers']['primary']['attribs']['ssl'])) {
            return true;
        }
        return false;
    }

    /**
     * @return string|false
     */
    function getSummary()
    {
        if (isset($this->_channelInfo['summary'])) {
            return $this->_channelInfo['summary'];
        } else {
            return false;
        }
    }

    /**
     * @param string xmlrpc or soap
     * @param string|false mirror name or false for primary server
     */
    function getPath($protocol, $mirror = false)
    {   
        if (!in_array($protocol, array('xmlrpc', 'soap'))) {
            return false;
        }
        if ($mirror) {
            if (!($mir = $this->getMirror($mirror))) {
                return false;
            }
            if (isset($mir[$protocol]['attribs']['path'])) {
                return $mir[$protocol]['attribs']['path'];
            } else {
                return $protocol . '.php';
            }
        } elseif (isset($this->_channelInfo['servers']['primary'][$protocol]['attribs']['path'])) {
            return $this->_channelInfo['servers']['primary'][$protocol]['attribs']['path'];
        }
        return $protocol . '.php';
    }

    /**
     * @param string protocol type (xmlrpc, soap)
     * @param string Mirror name
     * @return array|false
     */
    function getFunctions($protocol, $mirror = false)
    {
        if ($this->getName() == '__uri') {
            return false;
        }
        if ($protocol == 'rest') {
            $function = 'baseurl';
        } else {
            $function = 'function';
        }
        if ($mirror) {
            if ($mir = $this->getMirror($mirror)) {
                if (isset($mir[$protocol][$function])) {
                    return $mir[$protocol][$function];
                }
            }
            return false;
        }
        if (isset($this->_channelInfo['servers']['primary'][$protocol][$function])) {
            return $this->_channelInfo['servers']['primary'][$protocol][$function];
        } else {
            return false;
        }
    }

    /**
     * @param string Protocol type
     * @param string Function name (null to return the
     *               first protocol of the type requested)
     * @param string Mirror name, if any
     * @return array
     */
     function getFunction($type, $name = null, $mirror = false)
     {
        $protocols = $this->getFunctions($type, $mirror);
        if (!$protocols) {
            return false;
        }
        foreach ($protocols as $protocol) {
            if ($name === null) {
                return $protocol;
            }
            if ($protocol['_content'] != $name) {
                continue;
            }
            return $protocol;
        }
        return false;
     }

    /**
     * @param string protocol type
     * @param string protocol name
     * @param string version
     * @param string mirror name
     * @return boolean
     */
    function supports($type, $name = null, $mirror = false, $version = '1.0')
    {
        $protocols = $this->getFunctions($type, $mirror);
        if (!$protocols) {
            return false;
        }
        foreach ($protocols as $protocol) {
            if ($protocol['attribs']['version'] != $version) {
                continue;
            }
            if ($name === null) {
                return true;
            }
            if ($protocol['_content'] != $name) {
                continue;
            }
            return true;
        }
        return false;
    }

    /**
     * Determines whether a channel supports Representational State Transfer (REST) protocols
     * for retrieving channel information
     * @param string
     * @return bool
     */
    function supportsREST($mirror = false)
    {
        if ($mirror == $this->_channelInfo['name']) {
            $mirror = false;
        }
        if ($mirror) {
            if ($mir = $this->getMirror($mirror)) {
                return isset($mir['rest']);
            }
            return false;
        }
        return isset($this->_channelInfo['servers']['primary']['rest']);
    }

    /**
     * Get the URL to access a base resource.
     *
     * Hyperlinks in the returned xml will be used to retrieve the proper information
     * needed.  This allows extreme extensibility and flexibility in implementation
     * @param string Resource Type to retrieve
     */
    function getBaseURL($resourceType, $mirror = false)
    {
        if ($mirror == $this->_channelInfo['name']) {
            $mirror = false;
        }
        if ($mirror) {
            if ($mir = $this->getMirror($mirror)) {
                $rest = $mir['rest'];
            } else {
                return false;
            }
            $server = $mirror;
        } else {
            $rest = $this->_channelInfo['servers']['primary']['rest'];
            $server = $this->getServer();
        }
        if (!isset($rest['baseurl'][0])) {
            $rest['baseurl'] = array($rest['baseurl']);
        }
        foreach ($rest['baseurl'] as $baseurl) {
            if (strtolower($baseurl['attribs']['type']) == strtolower($resourceType)) {
                return $baseurl['_content'];
            }
        }
        return false;
    }

    /**
     * Since REST does not implement RPC, provide this as a logical wrapper around
     * resetFunctions for REST
     * @param string|false mirror name, if any
     */
    function resetREST($mirror = false)
    {
        return $this->resetFunctions('rest', $mirror);
    }

    /**
     * Empty all protocol definitions
     * @param string protocol type (xmlrpc, soap)
     * @param string|false mirror name, if any
     */
    function resetFunctions($type, $mirror = false)
    {
        if ($mirror) {
            if (isset($this->_channelInfo['servers']['mirror'])) {
                $mirrors = $this->_channelInfo['servers']['mirror'];
                if (!isset($mirrors[0])) {
                    $mirrors = array($mirrors);
                }
                foreach ($mirrors as $i => $mir) {
                    if ($mir['attribs']['host'] == $mirror) {
                        if (isset($this->_channelInfo['servers']['mirror'][$i][$type])) {
                            unset($this->_channelInfo['servers']['mirror'][$i][$type]);
                        }
                        return true;
                    }
                }
                return false;
            } else {
                return false;
            }
        } else {
            if (isset($this->_channelInfo['servers']['primary'][$type])) {
                unset($this->_channelInfo['servers']['primary'][$type]);
            }
            return true;
        }
    }

    /**
     * Set a channel's protocols to the protocols supported by pearweb
     */
    function setDefaultPEARProtocols($version = '1.0', $mirror = false)
    {
        switch ($version) {
            case '1.0' :
                $this->resetFunctions('xmlrpc', $mirror);
                $this->resetFunctions('soap', $mirror);
                $this->resetREST($mirror);
                $this->addFunction('xmlrpc', '1.0', 'logintest', $mirror);
                $this->addFunction('xmlrpc', '1.0', 'package.listLatestReleases', $mirror);
                $this->addFunction('xmlrpc', '1.0', 'package.listAll', $mirror);
                $this->addFunction('xmlrpc', '1.0', 'package.info', $mirror);
                $this->addFunction('xmlrpc', '1.0', 'package.getDownloadURL', $mirror);
                $this->addFunction('xmlrpc', '1.1', 'package.getDownloadURL', $mirror);
                $this->addFunction('xmlrpc', '1.0', 'package.getDepDownloadURL', $mirror);
                $this->addFunction('xmlrpc', '1.1', 'package.getDepDownloadURL', $mirror);
                $this->addFunction('xmlrpc', '1.0', 'package.search', $mirror);
                $this->addFunction('xmlrpc', '1.0', 'channel.listAll', $mirror);
                return true;
            break;
            default :
                return false;
            break;
        }
    }
    
    /**
     * @return array
     */
    function getMirrors()
    {
        if (isset($this->_channelInfo['servers']['mirror'])) {
            $mirrors = $this->_channelInfo['servers']['mirror'];
            if (!isset($mirrors[0])) {
                $mirrors = array($mirrors);
            }
            return $mirrors;
        } else {
            return array();
        }
    }

    /**
     * Get the unserialized XML representing a mirror
     * @return array|false
     */
    function getMirror($server)
    {
        foreach ($this->getMirrors() as $mirror) {
            if ($mirror['attribs']['host'] == $server) {
                return $mirror;
            }
        }
        return false;
    }

    /**
     * @param string
     * @return string|false
     * @error PEAR_CHANNELFILE_ERROR_NO_NAME
     * @error PEAR_CHANNELFILE_ERROR_INVALID_NAME
     */
    function setName($name)
    {
        return $this->setServer($name);
    }

    /**
     * Set the socket number (port) that is used to connect to this channel
     * @param integer
     * @param string|false name of the mirror server, or false for the primary
     */
    function setPort($port, $mirror = false)
    {
        if ($mirror) {
            if (!isset($this->_channelInfo['servers']['mirror'])) {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_MIRROR_NOT_FOUND,
                    array('mirror' => $mirror));
                return false;
            }
            $setmirror = false;
            if (isset($this->_channelInfo['servers']['mirror'][0])) {
                foreach ($this->_channelInfo['servers']['mirror'] as $i => $mir) {
                    if ($mirror == $mir['attribs']['host']) {
                        $this->_channelInfo['servers']['mirror'][$i]['attribs']['port'] = $port;
                        return true;
                    }
                }
                return false;
            } elseif ($this->_channelInfo['servers']['mirror']['attribs']['host'] == $mirror) {
                $this->_channelInfo['servers']['mirror']['attribs']['port'] = $port;
                $this->_isValid = false;
                return true;
            }
        }
        $this->_channelInfo['servers']['primary']['attribs']['port'] = $port;
        $this->_isValid = false;
        return true;
    }

    /**
     * Set the socket number (port) that is used to connect to this channel
     * @param bool Determines whether to turn on SSL support or turn it off
     * @param string|false name of the mirror server, or false for the primary
     */
    function setSSL($ssl = true, $mirror = false)
    {
        if ($mirror) {
            if (!isset($this->_channelInfo['servers']['mirror'])) {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_MIRROR_NOT_FOUND,
                    array('mirror' => $mirror));
                return false;
            }
            $setmirror = false;
            if (isset($this->_channelInfo['servers']['mirror'][0])) {
                foreach ($this->_channelInfo['servers']['mirror'] as $i => $mir) {
                    if ($mirror == $mir['attribs']['host']) {
                        if (!$ssl) {
                            if (isset($this->_channelInfo['servers']['mirror'][$i]
                                  ['attribs']['ssl'])) {
                                unset($this->_channelInfo['servers']['mirror'][$i]['attribs']['ssl']);
                            }
                        } else {
                            $this->_channelInfo['servers']['mirror'][$i]['attribs']['ssl'] = 'yes';
                        }
                        return true;
                    }
                }
                return false;
            } elseif ($this->_channelInfo['servers']['mirror']['attribs']['host'] == $mirror) {
                if (!$ssl) {
                    if (isset($this->_channelInfo['servers']['mirror']['attribs']['ssl'])) {
                        unset($this->_channelInfo['servers']['mirror']['attribs']['ssl']);
                    }
                } else {
                    $this->_channelInfo['servers']['mirror']['attribs']['ssl'] = 'yes';
                }
                $this->_isValid = false;
                return true;
            }
        }
        if ($ssl) {
            $this->_channelInfo['servers']['primary']['attribs']['ssl'] = 'yes';
        } else {
            if (isset($this->_channelInfo['servers']['primary']['attribs']['ssl'])) {
                unset($this->_channelInfo['servers']['primary']['attribs']['ssl']);
            }
        }
        $this->_isValid = false;
        return true;
    }

    /**
     * Set the socket number (port) that is used to connect to this channel
     * @param integer
     * @param string|false name of the mirror server, or false for the primary
     */
    function setPath($protocol, $path, $mirror = false)
    {
        if (!in_array($protocol, array('xmlrpc', 'soap'))) {
            return false;
        }
        if ($mirror) {
            if (!isset($this->_channelInfo['servers']['mirror'])) {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_MIRROR_NOT_FOUND,
                    array('mirror' => $mirror));
                return false;
            }
            $setmirror = false;
            if (isset($this->_channelInfo['servers']['mirror'][0])) {
                foreach ($this->_channelInfo['servers']['mirror'] as $i => $mir) {
                    if ($mirror == $mir['attribs']['host']) {
                        $this->_channelInfo['servers']['mirror'][$i][$protocol]['attribs']['path'] =
                            $path;
                        return true;
                    }
                }
                $this->_validateError(PEAR_CHANNELFILE_ERROR_MIRROR_NOT_FOUND,
                    array('mirror' => $mirror));
                return false;
            } elseif ($this->_channelInfo['servers']['mirror']['attribs']['host'] == $mirror) {
                $this->_channelInfo['servers']['mirror'][$protocol]['attribs']['path'] = $path;
                $this->_isValid = false;
                return true;
            }
        }
        $this->_channelInfo['servers']['primary'][$protocol]['attribs']['path'] = $path;
        $this->_isValid = false;
        return true;
    }

    /**
     * @param string
     * @return string|false
     * @error PEAR_CHANNELFILE_ERROR_NO_SERVER
     * @error PEAR_CHANNELFILE_ERROR_INVALID_SERVER
     */
    function setServer($server, $mirror = false)
    {
        if (empty($server)) {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_NO_SERVER);
            return false;
        } elseif (!$this->validChannelServer($server)) {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_INVALID_NAME,
                array('tag' => 'name', 'name' => $server));
            return false;
        }
        if ($mirror) {
            $found = false;
            foreach ($this->_channelInfo['servers']['mirror'] as $i => $mir) {
                if ($mirror == $mir['attribs']['host']) {
                    $found = true;
                    break;
                }
            }
            if (!$found) {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_MIRROR_NOT_FOUND,
                    array('mirror' => $mirror));
                return false;
            }
            $this->_channelInfo['mirror'][$i]['attribs']['host'] = $server;
            return true;
        }
        $this->_channelInfo['name'] = $server;
        return true;
    }

    /**
     * @param string
     * @return boolean success
     * @error PEAR_CHANNELFILE_ERROR_NO_SUMMARY
     * @warning PEAR_CHANNELFILE_ERROR_MULTILINE_SUMMARY
     */
    function setSummary($summary)
    {
        if (empty($summary)) {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_NO_SUMMARY);
            return false;
        } elseif (strpos(trim($summary), "\n") !== false) {
            $this->_validateWarning(PEAR_CHANNELFILE_ERROR_MULTILINE_SUMMARY,
                array('summary' => $summary));
        }
        $this->_channelInfo['summary'] = $summary;
        return true;
    }

    /**
     * @param string
     * @param boolean determines whether the alias is in channel.xml or local
     * @return boolean success
     */
    function setAlias($alias, $local = false)
    {
        if (!$this->validChannelServer($alias)) {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_INVALID_NAME,
                array('tag' => 'suggestedalias', 'name' => $alias));
            return false;
        }
        if ($local) {
            $this->_channelInfo['localalias'] = $alias;
        } else {
            $this->_channelInfo['suggestedalias'] = $alias;
        }
        return true;
    }

    /**
     * @return string
     */
    function getAlias()
    {
        if (isset($this->_channelInfo['localalias'])) {
            return $this->_channelInfo['localalias'];
        }
        if (isset($this->_channelInfo['suggestedalias'])) {
            return $this->_channelInfo['suggestedalias'];
        }
        if (isset($this->_channelInfo['name'])) {
            return $this->_channelInfo['name'];
        }
    }

    /**
     * Set the package validation object if it differs from PEAR's default
     * The class must be includeable via changing _ in the classname to path separator,
     * but no checking of this is made.
     * @param string|false pass in false to reset to the default packagename regex
     * @return boolean success
     */
    function setValidationPackage($validateclass, $version)
    {
        if (empty($validateclass)) {
            unset($this->_channelInfo['validatepackage']);
        }
        $this->_channelInfo['validatepackage'] = array('_content' => $validateclass);
        $this->_channelInfo['validatepackage']['attribs'] = array('version' => $version);
    }

    /**
     * Add a protocol to the provides section
     * @param string protocol type
     * @param string protocol version
     * @param string protocol name, if any
     * @param string mirror name, if this is a mirror's protocol
     * @return bool
     */
    function addFunction($type, $version, $name = '', $mirror = false)
    {
        if ($mirror) {
            return $this->addMirrorFunction($mirror, $type, $version, $name);
        }
        $set = array('attribs' => array('version' => $version), '_content' => $name);
        if (!isset($this->_channelInfo['servers']['primary'][$type]['function'])) {
            $this->_channelInfo['servers']['primary'][$type]['function'] = $set;
            $this->_isValid = false;
            return true;
        } elseif (!isset($this->_channelInfo['servers']['primary'][$type]['function'][0])) {
            $this->_channelInfo['servers']['primary'][$type]['function'] = array(
                $this->_channelInfo['servers']['primary'][$type]['function']);
        }
        $this->_channelInfo['servers']['primary'][$type]['function'][] = $set;
        return true;
    }
    /**
     * Add a protocol to a mirror's provides section
     * @param string mirror name (server)
     * @param string protocol type
     * @param string protocol version
     * @param string protocol name, if any
     */
    function addMirrorFunction($mirror, $type, $version, $name = '')
    {
        $found = false;
        if (!isset($this->_channelInfo['servers']['mirror'])) {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_MIRROR_NOT_FOUND,
                array('mirror' => $mirror));
            return false;
        }
        $setmirror = false;
        if (isset($this->_channelInfo['servers']['mirror'][0])) {
            foreach ($this->_channelInfo['servers']['mirror'] as $i => $mir) {
                if ($mirror == $mir['attribs']['host']) {
                    $setmirror = &$this->_channelInfo['servers']['mirror'][$i];
                    break;
                }
            }
        } else {
            if ($this->_channelInfo['servers']['mirror']['attribs']['host'] == $mirror) {
                $setmirror = &$this->_channelInfo['servers']['mirror'];
            }
        }
        if (!$setmirror) {
            $this->_validateError(PEAR_CHANNELFILE_ERROR_MIRROR_NOT_FOUND,
                array('mirror' => $mirror));
            return false;
        }
        $set = array('attribs' => array('version' => $version), '_content' => $name);
        if (!isset($setmirror[$type]['function'])) {
            $setmirror[$type]['function'] = $set;
            $this->_isValid = false;
            return true;
        } elseif (!isset($setmirror[$type]['function'][0])) {
            $setmirror[$type]['function'] = array($setmirror[$type]['function']);
        }
        $setmirror[$type]['function'][] = $set;
        $this->_isValid = false;
        return true;
    }

    /**
     * @param string Resource Type this url links to
     * @param string URL
     * @param string|false mirror name, if this is not a primary server REST base URL
     */
    function setBaseURL($resourceType, $url, $mirror = false)
    {
        if ($mirror) {
            $found = false;
            if (!isset($this->_channelInfo['servers']['mirror'])) {
                $this->_validateError(PEAR_CHANNELFILE_ERROR_MIRROR_NOT_FOUND,
                    array('mirror' => $mirror));
                return false;
            }
            $setmirror = false;
            if (isset($this->_channelInfo['servers']['mirror'][0])) {
                foreach ($this->_channelInfo['servers']['mirror'] as $i => $mir) {
                    if ($mirror == $mir['attribs']['host']) {
                        $setmirror = &$this->_channelInfo['servers']['mirror'][$i];
                        break;
                    }
                }
            } else {
                if ($this->_channelInfo['servers']['mirror']['attribs']['host'] == $mirror) {
                    $setmirror = &$this->_channelInfo['servers']['mirror'];
                }
            }
        } else {
            $setmirror = &$this->_channelInfo['servers']['primary'];
        }
        $set = array('attribs' => array('type' => $resourceType), '_content' => $url);
        if (!isset($setmirror['rest']['baseurl'])) {
            $setmirror['rest']['baseurl'] = $set;
            $this->_isValid = false;
            return true;
        } elseif (!isset($setmirror['rest']['baseurl'][0])) {
            $setmirror['rest']['baseurl'] = array($setmirror['rest']['baseurl']);
        }
        foreach ($setmirror['rest']['baseurl'] as $i => $url) {
            if ($url['attribs']['type'] == $resourceType) {
                $this->_isValid = false;
                $setmirror['rest']['baseurl'][$i] = $set;
                return true;
            }
        }
        $setmirror['rest']['baseurl'][] = $set;
        $this->_isValid = false;
        return true;
    }

    /**
     * @param string mirror server
     * @param int mirror http port
     * @return boolean
     */
    function addMirror($server, $port = null)
    {
        if ($this->_channelInfo['name'] == '__uri') {
            return false; // the __uri channel cannot have mirrors by definition
        }
        $set = array('attribs' => array('host' => $server));
        if (is_numeric($port)) {
            $set['attribs']['port'] = $port;
        }
        if (!isset($this->_channelInfo['servers']['mirror'])) {
            $this->_channelInfo['servers']['mirror'] = $set;
            return true;
        } else {
            if (!isset($this->_channelInfo['servers']['mirror'][0])) {
                $this->_channelInfo['servers']['mirror'] =
                    array($this->_channelInfo['servers']['mirror']);
            }
        }
        $this->_channelInfo['servers']['mirror'][] = $set;
        return true;
    }

    /**
     * Retrieve the name of the validation package for this channel
     * @return string|false
     */
    function getValidationPackage()
    {
        if (!$this->_isValid && !$this->validate()) {
            return false;
        }
        if (!isset($this->_channelInfo['validatepackage'])) {
            return array('attribs' => array('version' => 'default'),
                '_content' => 'PEAR_Validate');
        }
        return $this->_channelInfo['validatepackage'];
    }

    /**
     * Retrieve the object that can be used for custom validation
     * @param string|false the name of the package to validate.  If the package is
     *                     the channel validation package, PEAR_Validate is returned
     * @return PEAR_Validate|false false is returned if the validation package
     *         cannot be located
     */
    function &getValidationObject($package = false)
    {
        if (!class_exists('PEAR_Validate')) {
            require_once 'PEAR/Validate.php';
        }
        if (!$this->_isValid) {
            if (!$this->validate()) {
                $a = false;
                return $a;
            }
        }
        if (isset($this->_channelInfo['validatepackage'])) {
            if ($package == $this->_channelInfo['validatepackage']) {
                // channel validation packages are always validated by PEAR_Validate
                $val = &new PEAR_Validate;
                return $val;
            }
            if (!class_exists(str_replace('.', '_',
                  $this->_channelInfo['validatepackage']['_content']))) {
                if ($this->isIncludeable(str_replace('_', '/',
                      $this->_channelInfo['validatepackage']['_content']) . '.php')) {
                    include_once str_replace('_', '/',
                        $this->_channelInfo['validatepackage']['_content']) . '.php';
                    $vclass = str_replace('.', '_',
                        $this->_channelInfo['validatepackage']['_content']);
                    $val = &new $vclass;
                } else {
                    $a = false;
                    return $a;
                }
            } else {
                $vclass = str_replace('.', '_',
                    $this->_channelInfo['validatepackage']['_content']);
                $val = &new $vclass;
            }
        } else {
            $val = &new PEAR_Validate;
        }
        return $val;
    }

    function isIncludeable($path)
    {
        $possibilities = explode(PATH_SEPARATOR, ini_get('include_path'));
        foreach ($possibilities as $dir) {
            if (file_exists($dir . DIRECTORY_SEPARATOR . $path)
                  && is_readable($dir . DIRECTORY_SEPARATOR . $path)) {
                return true;
            }
        }
        return false;
    }

    /**
     * This function is used by the channel updater and retrieves a value set by
     * the registry, or the current time if it has not been set
     * @return string
     */
    function lastModified()
    {
        if (isset($this->_channelInfo['_lastmodified'])) {
            return $this->_channelInfo['_lastmodified'];
        }
        return time();
    }
}
?>
