<?php
/**
 * PEAR_ChannelFile_Parser for parsing channel.xml
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
 * base xml parser class
 */
require_once 'PEAR/XMLParser.php';
require_once 'PEAR/ChannelFile.php';
/**
 * Parser for channel.xml
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_ChannelFile_Parser extends PEAR_XMLParser
{
    var $_config;
    var $_logger;
    var $_registry;

    function setConfig(&$c)
    {
        $this->_config = &$c;
        $this->_registry = &$c->getRegistry();
    }

    function setLogger(&$l)
    {
        $this->_logger = &$l;
    }

    function parse($data, $file)
    {
        if (PEAR::isError($err = parent::parse($data, $file))) {
            return $err;
        }
        $ret = new PEAR_ChannelFile;
        $ret->setConfig($this->_config);
        if (isset($this->_logger)) {
            $ret->setLogger($this->_logger);
        }
        $ret->fromArray($this->_unserializedData);
        // make sure the filelist is in the easy to read format needed
        $ret->flattenFilelist();
        $ret->setPackagefile($file, $archive);
        return $ret;
    }
}
?>