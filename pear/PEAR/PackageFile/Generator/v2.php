<?php
/**
 * package.xml generation class, package.xml version 2.0
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
 * @author     Stephan Schmidt (original XML_Serializer code)
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 1.4.0a1
 */
/**
 * file/dir manipulation routines
 */
require_once 'System.php';
/**
 * This class converts a PEAR_PackageFile_v2 object into any output format.
 *
 * Supported output formats include array, XML string (using S. Schmidt's
 * XML_Serializer, slightly customized)
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @author     Stephan Schmidt (original XML_Serializer code)
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @PEAR-VER@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_PackageFile_Generator_v2
{
   /**
    * default options for the serialization
    * @access private
    * @var array $_defaultOptions
    */
    var $_defaultOptions = array(
                         'indent'             => ' ',                    // string used for indentation
                         'linebreak'          => "\n",                  // string used for newlines
                         'typeHints'          => false,                 // automatically add type hin attributes
                         'addDecl'            => true,                 // add an XML declaration
                         'defaultTagName'     => 'XML_Serializer_Tag',  // tag used for indexed arrays or invalid names
                         'classAsTagName'     => false,                 // use classname for objects in indexed arrays
                         'keyAttribute'       => '_originalKey',        // attribute where original key is stored
                         'typeAttribute'      => '_type',               // attribute for type (only if typeHints => true)
                         'classAttribute'     => '_class',              // attribute for class of objects (only if typeHints => true)
                         'scalarAsAttributes' => false,                 // scalar values (strings, ints,..) will be serialized as attribute
                         'prependAttributes'  => '',                    // prepend string for attributes
                         'indentAttributes'   => false,                 // indent the attributes, if set to '_auto', it will indent attributes so they all start at the same column
                         'mode'               => 'simplexml',             // use 'simplexml' to use parent name as tagname if transforming an indexed array
                         'addDoctype'         => false,                 // add a doctype declaration
                         'doctype'            => null,                  // supply a string or an array with id and uri ({@see XML_Util::getDoctypeDeclaration()}
                         'rootName'           => 'package',                  // name of the root tag
                         'rootAttributes'     => array(
                             'version' => '2.0',
                             'xmlns' => 'http://pear.php.net/dtd/package-2.0',
                             'xmlns:tasks' => 'http://pear.php.net/dtd/tasks-1.0',
                             'xmlns:xsi' => 'http://www.w3.org/2001/XMLSchema-instance',
                             'xsi:schemaLocation' => 'http://pear.php.net/dtd/tasks-1.0
http://pear.php.net/dtd/tasks-1.0.xsd
http://pear.php.net/dtd/package-2.0
http://pear.php.net/dtd/package-2.0.xsd',
                         ),               // attributes of the root tag
                         'attributesArray'    => 'attribs',                  // all values in this key will be treated as attributes
                         'contentName'        => '_content',                   // this value will be used directly as content, instead of creating a new tag, may only be used in conjuction with attributesArray
                         'beautifyFilelist'   => false,
                         'encoding' => 'UTF-8',
                        );

   /**
    * options for the serialization
    * @access private
    * @var array $options
    */
    var $options = array();

   /**
    * current tag depth
    * @var integer $_tagDepth
    */
    var $_tagDepth = 0;

   /**
    * serilialized representation of the data
    * @var string $_serializedData
    */
    var $_serializedData = null;
    /**
     * @var PEAR_PackageFile_v2
     */
    var $_packagefile;
    /**
     * @param PEAR_PackageFile_v2
     */
    function PEAR_PackageFile_Generator_v2(&$packagefile)
    {
        $this->_packagefile = &$packagefile;
    }

    /**
     * @return string
     */
    function getPackagerVersion()
    {
        return '@PEAR-VER@';
    }

    /**
     * @param PEAR_Packager
     * @param bool generate a .tgz or a .tar
     * @param string|null temporary directory to package in
     */
    function toTgz(&$packager, $compress = true, $where = null)
    {
        $a = null;
        return $this->toTgz2($packager, $a, $compress, $where);
    }

    /**
     * Package up both a package.xml and package2.xml for the same release
     * @param PEAR_Packager
     * @param PEAR_PackageFile_v1
     * @param bool generate a .tgz or a .tar
     * @param string|null temporary directory to package in
     */
    function toTgz2(&$packager, &$pf1, $compress = true, $where = null)
    {
        require_once 'Archive/Tar.php';
        if (!$this->_packagefile->isEquivalent($pf1)) {
            return PEAR::raiseError('PEAR_Packagefile_v2::toTgz: "' .
                basename($pf1->getPackageFile()) .
                '" is not equivalent to "' . basename($this->_packagefile->getPackageFile())
                . '"');
        }
        if ($where === null) {
            if (!($where = System::mktemp(array('-d')))) {
                return PEAR::raiseError('PEAR_Packagefile_v2::toTgz: mktemp failed');
            }
        } elseif (!@System::mkDir(array('-p', $where))) {
            return PEAR::raiseError('PEAR_Packagefile_v2::toTgz: "' . $where . '" could' .
                ' not be created');
        }
        if (file_exists($where . DIRECTORY_SEPARATOR . 'package.xml') &&
              !is_file($where . DIRECTORY_SEPARATOR . 'package.xml')) {
            return PEAR::raiseError('PEAR_Packagefile_v2::toTgz: unable to save package.xml as' .
                ' "' . $where . DIRECTORY_SEPARATOR . 'package.xml"');
        }
        if (!$this->_packagefile->validate(PEAR_VALIDATE_PACKAGING)) {
            return PEAR::raiseError('PEAR_Packagefile_v2::toTgz: invalid package.xml');
        }
        $ext = $compress ? '.tgz' : '.tar';
        $pkgver = $this->_packagefile->getPackage() . '-' . $this->_packagefile->getVersion();
        $dest_package = getcwd() . DIRECTORY_SEPARATOR . $pkgver . $ext;
        if (file_exists(getcwd() . DIRECTORY_SEPARATOR . $pkgver . $ext) &&
              !is_file(getcwd() . DIRECTORY_SEPARATOR . $pkgver . $ext)) {
            return PEAR::raiseError('PEAR_Packagefile_v2::toTgz: cannot create tgz file "' .
                getcwd() . DIRECTORY_SEPARATOR . $pkgver . $ext . '"');
        }
        if ($pkgfile = $this->_packagefile->getPackageFile()) {
            $pkgdir = dirname(realpath($pkgfile));
            $pkgfile = basename($pkgfile);
        } else {
            return PEAR::raiseError('PEAR_Packagefile_v2::toTgz: package file object must ' .
                'be created from a real file');
        }
        // {{{ Create the package file list
        $filelist = array();
        $i = 0;
        $this->_packagefile->flattenFilelist();
        $contents = $this->_packagefile->getContents();
        if (isset($contents['bundledpackage'])) { // bundles of packages
            $contents = $contents['bundledpackage'];
            if (!isset($contents[0])) {
                $contents = array($contents);
            }
            $packageDir = $where;
            foreach ($contents as $i => $package) {
                $fname = $package;
                $file = $pkgdir . DIRECTORY_SEPARATOR . $fname;
                if (!file_exists($file)) {
                    return $packager->raiseError("File does not exist: $fname");
                }
                $tfile = $packageDir . DIRECTORY_SEPARATOR . $fname;
                System::mkdir(array('-p', dirname($tfile)));
                copy($file, $tfile);
                $filelist[$i++] = $tfile;
                $packager->log(2, "Adding package $fname");
            }
        } else { // normal packages
            $contents = $contents['dir']['file'];
            if (!isset($contents[0])) {
                $contents = array($contents);
            }
    
            $packageDir = $where;
            foreach ($contents as $i => $file) {
                $fname = $file['attribs']['name'];
                $atts = $file['attribs'];
                $orig = $file;
                $file = $pkgdir . DIRECTORY_SEPARATOR . $fname;
                if (!file_exists($file)) {
                    return $packager->raiseError("File does not exist: $fname");
                } else {
                    $tfile = $packageDir . DIRECTORY_SEPARATOR . $fname;
                    unset($orig['attribs']);
                    if (count($orig)) { // file with tasks
                        // run any package-time tasks
                        if (function_exists('file_get_contents')) {
                            $contents = file_get_contents($file);
                        } else {
                            $fp = fopen($file, "r");
                            $contents = @fread($fp, filesize($file));
                            fclose($fp);
                        }
                        foreach ($orig as $tag => $raw) {
                            $tag = str_replace($this->_packagefile->getTasksNs() . ':', '', $tag);
                            $task = "PEAR_Task_$tag";
                            $task = &new $task($this->_packagefile->_config,
                                $this->_packagefile->_logger,
                                PEAR_TASK_PACKAGE);
                            $task->init($raw, $atts, null);
                            $res = $task->startSession($this->_packagefile, $contents, $tfile);
                            if (!$res) {
                                continue; // skip this task
                            }
                            if (PEAR::isError($res)) {
                                return $res;
                            }
                            $contents = $res; // save changes
                            System::mkdir(array('-p', dirname($tfile)));
                            $wp = fopen($tfile, "wb");
                            fwrite($wp, $contents);
                            fclose($wp);
                        }
                    }
                    if (!file_exists($tfile)) {
                        System::mkdir(array('-p', dirname($tfile)));
                        copy($file, $tfile);
                    }
                    $filelist[$i++] = $tfile;
                    $this->_packagefile->setFileAttribute($fname, 'md5sum', md5_file($tfile), $i - 1);
                    $packager->log(2, "Adding file $fname");
                }
            }
        }
            // }}}
        if ($pf1 !== null) {
            $name = 'package2.xml';
        } else {
            $name = 'package.xml';
        }
        $packagexml = $this->toPackageFile($where, PEAR_VALIDATE_PACKAGING, $name);
        if ($packagexml) {
            $tar =& new Archive_Tar($dest_package, $compress);
            $tar->setErrorHandling(PEAR_ERROR_RETURN); // XXX Don't print errors
            // ----- Creates with the package.xml file
            $ok = $tar->createModify(array($packagexml), '', $where);
            if (PEAR::isError($ok)) {
                return $packager->raiseError($ok);
            } elseif (!$ok) {
                return $packager->raiseError('PEAR_Packagefile_v2::toTgz(): adding ' . $name .
                    ' failed');
            }
            // ----- Add the content of the package
            if (!$tar->addModify($filelist, $pkgver, $where)) {
                return $packager->raiseError(
                    'PEAR_Packagefile_v2::toTgz(): tarball creation failed');
            }
            // add the package.xml version 1.0
            if ($pf1 !== null) {
                $pfgen = &$pf1->getDefaultGenerator();
                $packagexml1 = $pfgen->toPackageFile($where, PEAR_VALIDATE_PACKAGING,
                    'package.xml', true);
                if (!$tar->addModify(array($packagexml1), '', $where)) {
                    return $packager->raiseError(
                        'PEAR_Packagefile_v2::toTgz(): adding package.xml failed');
                }
            }
            return $dest_package;
        }
    }

    function toPackageFile($where = null, $state = PEAR_VALIDATE_NORMAL, $name = 'package.xml')
    {
        if (!$this->_packagefile->validate($state)) {
            return PEAR::raiseError('PEAR_Packagefile_v2::toPackageFile: invalid package.xml',
                null, null, null, $this->_packagefile->getValidationWarnings());
        }
        if ($where === null) {
            if (!($where = System::mktemp(array('-d')))) {
                return PEAR::raiseError('PEAR_Packagefile_v2::toPackageFile: mktemp failed');
            }
        } elseif (!@System::mkDir(array('-p', $where))) {
            return PEAR::raiseError('PEAR_Packagefile_v2::toPackageFile: "' . $where . '" could' .
                ' not be created');
        }
        $newpkgfile = $where . DIRECTORY_SEPARATOR . $name;
        $np = @fopen($newpkgfile, 'wb');
        if (!$np) {
            return PEAR::raiseError('PEAR_Packagefile_v2::toPackageFile: unable to save ' .
               "$name as $newpkgfile");
        }
        fwrite($np, $this->toXml($state));
        fclose($np);
        return $newpkgfile;
    }

    function &toV2()
    {
        return $this->_packagefile;
    }

    /**
     * Return an XML document based on the package info (as returned
     * by the PEAR_Common::infoFrom* methods).
     *
     * @return string XML data
     */
    function toXml($state = PEAR_VALIDATE_NORMAL, $options = array())
    {
        $this->_packagefile->setDate(date('Y-m-d'));
        $this->_packagefile->setTime(date('H:i:s'));
        if (!$this->_packagefile->validate($state)) {
            return false;
        }
        if (is_array($options)) {
            $this->options = array_merge($this->_defaultOptions, $options);
        } else {
            $this->options = $this->_defaultOptions;
        }
        $arr = $this->_packagefile->getArray();
        if (isset($arr['filelist'])) {
            unset($arr['filelist']);
        }
        if (isset($arr['_lastversion'])) {
            unset($arr['_lastversion']);
        }
        if ($state ^ PEAR_VALIDATE_PACKAGING && !isset($arr['bundle'])) {
            $use = $this->_recursiveXmlFilelist($arr['contents']['dir']['file']);
            unset($arr['contents']['dir']['file']);
            if (isset($use['dir'])) {
                $arr['contents']['dir']['dir'] = $use['dir'];
            }
            if (isset($use['file'])) {
                $arr['contents']['dir']['file'] = $use['file'];
            }
            $this->options['beautifyFilelist'] = true;
        }
        $arr['attribs']['packagerversion'] = '@PEAR-VER@';
        if ($this->serialize($arr, $options)) {
            return $this->_serializedData . "\n";
        }
        return false;
    }


    function _recursiveXmlFilelist($list)
    {
        $dirs = array();
        if (isset($list['attribs'])) {
            $file = $list['attribs']['name'];
            unset($list['attribs']['name']);
            $attributes = $list['attribs'];
            $this->_addDir($dirs, explode('/', dirname($file)), $file, $attributes);
        } else {
            foreach ($list as $a) {
                $file = $a['attribs']['name'];
                $attributes = $a['attribs'];
                unset($a['attribs']);
                $this->_addDir($dirs, explode('/', dirname($file)), $file, $attributes, $a);
            }
        }
        $this->_formatDir($dirs);
        $this->_deFormat($dirs);
        return $dirs;
    }

    function _addDir(&$dirs, $dir, $file = null, $attributes = null, $tasks = null)
    {
        if (!$tasks) {
            $tasks = array();
        }
        if ($dir == array() || $dir == array('.')) {
            $dirs['file'][basename($file)] = $tasks;
            $attributes['name'] = basename($file);
            $dirs['file'][basename($file)]['attribs'] = $attributes;
            return;
        }
        $curdir = array_shift($dir);
        if (!isset($dirs['dir'][$curdir])) {
            $dirs['dir'][$curdir] = array();
        }
        $this->_addDir($dirs['dir'][$curdir], $dir, $file, $attributes, $tasks);
    }

    function _formatDir(&$dirs)
    {
        if (!count($dirs)) {
            return array();
        }
        $newdirs = array();
        if (isset($dirs['dir'])) {
            $newdirs['dir'] = $dirs['dir'];
        }
        if (isset($dirs['file'])) {
            $newdirs['file'] = $dirs['file'];
        }
        $dirs = $newdirs;
        if (isset($dirs['dir'])) {
            uksort($dirs['dir'], 'strnatcasecmp');
            foreach ($dirs['dir'] as $dir => $contents) {
                $this->_formatDir($dirs['dir'][$dir]);
            }
        }
        if (isset($dirs['file'])) {
            uksort($dirs['file'], 'strnatcasecmp');
        };
    }

    function _deFormat(&$dirs)
    {
        if (!count($dirs)) {
            return array();
        }
        $newdirs = array();
        if (isset($dirs['dir'])) {
            foreach ($dirs['dir'] as $dir => $contents) {
                $newdir = array();
                $newdir['attribs']['name'] = $dir;
                $this->_deFormat($contents);
                foreach ($contents as $tag => $val) {
                    $newdir[$tag] = $val;
                }
                $newdirs['dir'][] = $newdir;
            }
            if (count($newdirs['dir']) == 1) {
                $newdirs['dir'] = $newdirs['dir'][0];
            }
        }
        if (isset($dirs['file'])) {
            foreach ($dirs['file'] as $name => $file) {
                $newdirs['file'][] = $file;
            }
            if (count($newdirs['file']) == 1) {
                $newdirs['file'] = $newdirs['file'][0];
            }
        }
        $dirs = $newdirs;
    }

    /**
    * reset all options to default options
    *
    * @access   public
    * @see      setOption(), XML_Unserializer()
    */
    function resetOptions()
    {
        $this->options = $this->_defaultOptions;
    }

   /**
    * set an option
    *
    * You can use this method if you do not want to set all options in the constructor
    *
    * @access   public
    * @see      resetOption(), XML_Serializer()
    */
    function setOption($name, $value)
    {
        $this->options[$name] = $value;
    }
    
   /**
    * sets several options at once
    *
    * You can use this method if you do not want to set all options in the constructor
    *
    * @access   public
    * @see      resetOption(), XML_Unserializer(), setOption()
    */
    function setOptions($options)
    {
        $this->options = array_merge($this->options, $options);
    }

   /**
    * serialize data
    *
    * @access   public
    * @param    mixed    $data data to serialize
    * @return   boolean  true on success, pear error on failure
    */
    function serialize($data, $options = null)
    {
        // if options have been specified, use them instead
        // of the previously defined ones
        if (is_array($options)) {
            $optionsBak = $this->options;
            if (isset($options['overrideOptions']) && $options['overrideOptions'] == true) {
                $this->options = array_merge($this->_defaultOptions, $options);
            } else {
                $this->options = array_merge($this->options, $options);
            }
        }
        else {
            $optionsBak = null;
        }
        
        //  start depth is zero
        $this->_tagDepth = 0;

        $this->_serializedData = '';
        // serialize an array
        if (is_array($data)) {
            if (isset($this->options['rootName'])) {
                $tagName = $this->options['rootName'];
            } else {
                $tagName = 'array';
            }

            $this->_serializedData .= $this->_serializeArray($data, $tagName, $this->options['rootAttributes']);
        }
        
        // add doctype declaration
        if ($this->options['addDoctype'] === true) {
            $this->_serializedData = XML_Util::getDoctypeDeclaration($tagName, $this->options['doctype'])
                                   . $this->options['linebreak']
                                   . $this->_serializedData;
        }

        //  build xml declaration
        if ($this->options['addDecl']) {
            $atts = array();
            if (isset($this->options['encoding']) ) {
                $encoding = $this->options['encoding'];
            } else {
                $encoding = null;
            }
            $this->_serializedData = XML_Util::getXMLDeclaration('1.0', $encoding)
                                   . $this->options['linebreak']
                                   . $this->_serializedData;
        }
        
        
		if ($optionsBak !== null) {
			$this->options = $optionsBak;
		}
		
        return  true;
    }

   /**
    * get the result of the serialization
    *
    * @access public
    * @return string serialized XML
    */
    function getSerializedData()
    {
        if ($this->_serializedData == null ) {
            return  $this->raiseError('No serialized data available. Use XML_Serializer::serialize() first.', XML_SERIALIZER_ERROR_NO_SERIALIZATION);
        }
        return $this->_serializedData;
    }
    
   /**
    * serialize any value
    *
    * This method checks for the type of the value and calls the appropriate method
    *
    * @access private
    * @param  mixed     $value
    * @param  string    $tagName
    * @param  array     $attributes
    * @return string
    */
    function _serializeValue($value, $tagName = null, $attributes = array())
    {
        if (is_array($value)) {
            $xml = $this->_serializeArray($value, $tagName, $attributes);
        } elseif (is_object($value)) {
            $xml = $this->_serializeObject($value, $tagName);
        } else {
            $tag = array(
                          'qname'      => $tagName,
                          'attributes' => $attributes,
                          'content'    => $value
                        );
            $xml = $this->_createXMLTag($tag);
        }
        return $xml;
    }
    
   /**
    * serialize an array
    *
    * @access   private
    * @param    array   $array       array to serialize
    * @param    string  $tagName     name of the root tag
    * @param    array   $attributes  attributes for the root tag
    * @return   string  $string      serialized data
    * @uses     XML_Util::isValidName() to check, whether key has to be substituted
    */
    function _serializeArray(&$array, $tagName = null, $attributes = array())
    {
        $_content = null;
        
        /**
         * check for special attributes
         */
        if ($this->options['attributesArray'] !== null) {
            if (isset($array[$this->options['attributesArray']])) {
                $attributes = $array[$this->options['attributesArray']];
                unset($array[$this->options['attributesArray']]);
            }
            /**
             * check for special content
             */
            if ($this->options['contentName'] !== null) {
                if (isset($array[$this->options['contentName']])) {
                    $_content = $array[$this->options['contentName']];
                    unset($array[$this->options['contentName']]);
                }
            }
        }

        /*
        * if mode is set to simpleXML, check whether
        * the array is associative or indexed
        */
        if (is_array($array) && $this->options['mode'] == 'simplexml') {
            $indexed = true;
            if (!count($array)) {
                $indexed = false;
            }
            foreach ($array as $key => $val) {
                if (!is_int($key)) {
                    $indexed = false;
                    break;
                }
            }

            if ($indexed && $this->options['mode'] == 'simplexml') {
                $string = '';
                foreach ($array as $key => $val) {
                    if ($this->options['beautifyFilelist'] && $tagName == 'dir') {
                        if (!isset($this->_curdir)) {
                            $this->_curdir = '';
                        }
                        $savedir = $this->_curdir;
                        if (isset($val['attribs'])) {
                            if ($val['attribs']['name'] == '/') {
                                $this->_curdir = '/';
                            } else {
                                if ($this->_curdir == '/') {
                                    $this->_curdir = '';
                                }
                                $this->_curdir .= '/' . $val['attribs']['name'];
                            }
                        }
                    }
                    $string .= $this->_serializeValue( $val, $tagName, $attributes);
                    if ($this->options['beautifyFilelist'] && $tagName == 'dir') {
                        $string .= ' <!-- ' . $this->_curdir . ' -->';
                        if (empty($savedir)) {
                            unset($this->_curdir);
                        } else {
                            $this->_curdir = $savedir;
                        }
                    }
                    
                    $string .= $this->options['linebreak'];
        			//	do indentation
                    if ($this->options['indent']!==null && $this->_tagDepth>0) {
                        $string .= str_repeat($this->options['indent'], $this->_tagDepth);
                    }
                }
                return rtrim($string);
            }
        }
        
		if ($this->options['scalarAsAttributes'] === true) {
	        foreach ($array as $key => $value) {
				if (is_scalar($value) && (XML_Util::isValidName($key) === true)) {
					unset($array[$key]);
					$attributes[$this->options['prependAttributes'].$key] = $value;
				}
			}
		}

        // check for empty array => create empty tag
        if (empty($array)) {
            $tag = array(
                            'qname'      => $tagName,
                            'content'    => $_content,
                            'attributes' => $attributes
                        );

        } else {
            $this->_tagDepth++;
            $tmp = $this->options['linebreak'];
            foreach ($array as $key => $value) {
    			//	do indentation
                if ($this->options['indent']!==null && $this->_tagDepth>0) {
                    $tmp .= str_repeat($this->options['indent'], $this->_tagDepth);
                }
    
    			//	copy key
    			$origKey	=	$key;
    			//	key cannot be used as tagname => use default tag
                $valid = XML_Util::isValidName($key);
    	        if (PEAR::isError($valid)) {
    	            if ($this->options['classAsTagName'] && is_object($value)) {
    	                $key = get_class($value);
    	            } else {
            	        $key = $this->options['defaultTagName'];
    	            }
           	 	}
                $atts = array();
                if ($this->options['typeHints'] === true) {
                    $atts[$this->options['typeAttribute']] = gettype($value);
    				if ($key !== $origKey) {
    					$atts[$this->options['keyAttribute']] = (string)$origKey;
    				}
    
                }
                if ($this->options['beautifyFilelist'] && $key == 'dir') {
                    if (!isset($this->_curdir)) {
                        $this->_curdir = '';
                    }
                    $savedir = $this->_curdir;
                    if (isset($value['attribs'])) {
                        if ($value['attribs']['name'] == '/') {
                            $this->_curdir = '/';
                        } else {
                            $this->_curdir .= '/' . $value['attribs']['name'];
                        }
                    }
                }

                if (is_string($value) && $value && ($value{strlen($value) - 1} == "\n")) {
                    $value .= str_repeat($this->options['indent'], $this->_tagDepth);
                }
                $tmp .= $this->_createXMLTag(array(
                                                    'qname'      => $key,
                                                    'attributes' => $atts,
                                                    'content'    => $value )
                                            );
                if ($this->options['beautifyFilelist'] && $key == 'dir') {
                    if (isset($value['attribs'])) {
                        $tmp .= ' <!-- ' . $this->_curdir . ' -->';
                        if (empty($savedir)) {
                            unset($this->_curdir);
                        } else {
                            $this->_curdir = $savedir;
                        }
                    }
                }
                $tmp .= $this->options['linebreak'];
            }
            
            $this->_tagDepth--;
            if ($this->options['indent']!==null && $this->_tagDepth>0) {
                $tmp .= str_repeat($this->options['indent'], $this->_tagDepth);
            }
    
    		if (trim($tmp) === '') {
    			$tmp = null;
    		}
    		
            $tag = array(
                            'qname'      => $tagName,
                            'content'    => $tmp,
                            'attributes' => $attributes
                        );
        }
        if ($this->options['typeHints'] === true) {
            if (!isset($tag['attributes'][$this->options['typeAttribute']])) {
                $tag['attributes'][$this->options['typeAttribute']] = 'array';
            }
        }

        $string = $this->_createXMLTag($tag, false);
        return $string;
    }
  
   /**
    * create a tag from an array
    * this method awaits an array in the following format
    * array(
    *       'qname'        => $tagName,
    *       'attributes'   => array(),
    *       'content'      => $content,      // optional
    *       'namespace'    => $namespace     // optional
    *       'namespaceUri' => $namespaceUri  // optional
    *   )
    *
    * @access   private
    * @param    array   $tag tag definition
    * @param    boolean $replaceEntities whether to replace XML entities in content or not
    * @return   string  $string XML tag
    */
    function _createXMLTag( $tag, $replaceEntities = true )
    {
        if ($this->options['indentAttributes'] !== false) {
            $multiline = true;
            $indent    = str_repeat($this->options['indent'], $this->_tagDepth);

            if ($this->options['indentAttributes'] == '_auto') {
                $indent .= str_repeat(' ', (strlen($tag['qname'])+2));

            } else {
                $indent .= $this->options['indentAttributes'];
            }
        } else {
            $multiline = false;
            $indent    = false;
        }
    
        if (is_array($tag['content'])) {
            if (empty($tag['content'])) {
                $tag['content'] =   '';
            }
        } elseif(is_scalar($tag['content']) && (string)$tag['content'] == '') {
            $tag['content'] =   '';
        }
    
        if (is_scalar($tag['content']) || is_null($tag['content'])) {
            if ($this->options['encoding'] == 'UTF-8' &&
                  version_compare(phpversion(), '5.0.0', 'lt')) {
                $encoding = XML_UTIL_ENTITIES_UTF8_XML;
            } else {
                $encoding = XML_UTIL_ENTITIES_XML;
            }
            $tag = XML_Util::createTagFromArray($tag, $replaceEntities, $multiline, $indent, $this->options['linebreak'], $encoding);
        } elseif (is_array($tag['content'])) {
            $tag    =   $this->_serializeArray($tag['content'], $tag['qname'], $tag['attributes']);
        } elseif (is_object($tag['content'])) {
            $tag    =   $this->_serializeObject($tag['content'], $tag['qname'], $tag['attributes']);
        } elseif (is_resource($tag['content'])) {
            settype($tag['content'], 'string');
            $tag    =   XML_Util::createTagFromArray($tag, $replaceEntities);
        }
        return  $tag;
    }
}

//foreach (explode(PATH_SEPARATOR, ini_get('include_path')) as $path) {
//    $t = $path . DIRECTORY_SEPARATOR . 'XML' . DIRECTORY_SEPARATOR .
//          'Util';
//    if (file_exists($t) && is_readable($t)) {
//        include_once 'XML/Util';
//    }
//}
//if (!class_exists('XML_Util')) {
// well, it's one way to do things without extra deps ...
/* vim: set expandtab tabstop=4 shiftwidth=4: */
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stephan Schmidt <schst@php-tools.net>                       |
// +----------------------------------------------------------------------+
//
//    $Id$

/**
 * error code for invalid chars in XML name
 */
define("XML_UTIL_ERROR_INVALID_CHARS", 51);

/**
 * error code for invalid chars in XML name
 */
define("XML_UTIL_ERROR_INVALID_START", 52);

/**
 * error code for non-scalar tag content
 */
define("XML_UTIL_ERROR_NON_SCALAR_CONTENT", 60);
    
/**
 * error code for missing tag name
 */
define("XML_UTIL_ERROR_NO_TAG_NAME", 61);
    
/**
 * replace XML entities
 */
define("XML_UTIL_REPLACE_ENTITIES", 1);

/**
 * embedd content in a CData Section
 */
define("XML_UTIL_CDATA_SECTION", 2);

/**
 * do not replace entitites
 */
define("XML_UTIL_ENTITIES_NONE", 0);

/**
 * replace all XML entitites
 * This setting will replace <, >, ", ' and &
 */
define("XML_UTIL_ENTITIES_XML", 1);

/**
 * replace only required XML entitites
 * This setting will replace <, " and &
 */
define("XML_UTIL_ENTITIES_XML_REQUIRED", 2);

/**
 * replace HTML entitites
 * @link    http://www.php.net/htmlentities
 */
define("XML_UTIL_ENTITIES_HTML", 3);

/**
 * replace all XML entitites, and encode from ISO-8859-1 to UTF-8
 * This setting will replace <, >, ", ' and &
 */
define("XML_UTIL_ENTITIES_UTF8_XML", 4);

/**
 * utility class for working with XML documents
 *
 * @category XML
 * @package  XML_Util
 * @version  0.6.0
 * @author   Stephan Schmidt <schst@php.net>
 */
class XML_Util {

   /**
    * return API version
    *
    * @access   public
    * @static
    * @return   string  $version API version
    */
    function apiVersion()
    {
        return "0.6";
    }

   /**
    * replace XML entities
    *
    * With the optional second parameter, you may select, which
    * entities should be replaced.
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // replace XML entites:
    * $string = XML_Util::replaceEntities("This string contains < & >.");
    * </code>
    *
    * @access   public
    * @static
    * @param    string  string where XML special chars should be replaced
    * @param    integer setting for entities in attribute values (one of XML_UTIL_ENTITIES_XML, XML_UTIL_ENTITIES_XML_REQUIRED, XML_UTIL_ENTITIES_HTML)
    * @return   string  string with replaced chars
    */
    function replaceEntities($string, $replaceEntities = XML_UTIL_ENTITIES_XML)
    {
        switch ($replaceEntities) {
            case XML_UTIL_ENTITIES_UTF8_XML:
                return strtr(utf8_encode($string),array(
                                          '&'  => '&amp;',
                                          '>'  => '&gt;',
                                          '<'  => '&lt;',
                                          '"'  => '&quot;',
                                          '\'' => '&apos;' ));
                break;
            case XML_UTIL_ENTITIES_XML:
                return strtr($string,array(
                                          '&'  => '&amp;',
                                          '>'  => '&gt;',
                                          '<'  => '&lt;',
                                          '"'  => '&quot;',
                                          '\'' => '&apos;' ));
                break;
            case XML_UTIL_ENTITIES_XML_REQUIRED:
                return strtr($string,array(
                                          '&'  => '&amp;',
                                          '<'  => '&lt;',
                                          '"'  => '&quot;' ));
                break;
            case XML_UTIL_ENTITIES_HTML:
                return htmlspecialchars($string);
                break;
        }
        return $string;
    }

   /**
    * build an xml declaration
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // get an XML declaration:
    * $xmlDecl = XML_Util::getXMLDeclaration("1.0", "UTF-8", true);
    * </code>
    *
    * @access   public
    * @static
    * @param    string  $version     xml version
    * @param    string  $encoding    character encoding
    * @param    boolean $standAlone  document is standalone (or not)
    * @return   string  $decl xml declaration
    * @uses     XML_Util::attributesToString() to serialize the attributes of the XML declaration
    */
    function getXMLDeclaration($version = "1.0", $encoding = null, $standalone = null)
    {
        $attributes = array(
                            "version" => $version,
                           );
        // add encoding
        if ($encoding !== null) {
            $attributes["encoding"] = $encoding;
        }
        // add standalone, if specified
        if ($standalone !== null) {
            $attributes["standalone"] = $standalone ? "yes" : "no";
        }
        
        return sprintf("<?xml%s?>", XML_Util::attributesToString($attributes, false));
    }

   /**
    * build a document type declaration
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // get a doctype declaration:
    * $xmlDecl = XML_Util::getDocTypeDeclaration("rootTag","myDocType.dtd");
    * </code>
    *
    * @access   public
    * @static
    * @param    string  $root         name of the root tag
    * @param    string  $uri          uri of the doctype definition (or array with uri and public id)
    * @param    string  $internalDtd  internal dtd entries   
    * @return   string  $decl         doctype declaration
    * @since    0.2
    */
    function getDocTypeDeclaration($root, $uri = null, $internalDtd = null)
    {
        if (is_array($uri)) {
            $ref = sprintf( ' PUBLIC "%s" "%s"', $uri["id"], $uri["uri"] );
        } elseif (!empty($uri)) {
            $ref = sprintf( ' SYSTEM "%s"', $uri );
        } else {
            $ref = "";
        }

        if (empty($internalDtd)) {
            return sprintf("<!DOCTYPE %s%s>", $root, $ref);
        } else {
            return sprintf("<!DOCTYPE %s%s [\n%s\n]>", $root, $ref, $internalDtd);
        }
    }

   /**
    * create string representation of an attribute list
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // build an attribute string
    * $att = array(
    *              "foo"   =>  "bar",
    *              "argh"  =>  "tomato"
    *            );
    *
    * $attList = XML_Util::attributesToString($att);    
    * </code>
    *
    * @access   public
    * @static
    * @param    array         $attributes        attribute array
    * @param    boolean|array $sort              sort attribute list alphabetically, may also be an assoc array containing the keys 'sort', 'multiline', 'indent', 'linebreak' and 'entities'
    * @param    boolean       $multiline         use linebreaks, if more than one attribute is given
    * @param    string        $indent            string used for indentation of multiline attributes
    * @param    string        $linebreak         string used for linebreaks of multiline attributes
    * @param    integer       $entities          setting for entities in attribute values (one of XML_UTIL_ENTITIES_NONE, XML_UTIL_ENTITIES_XML, XML_UTIL_ENTITIES_XML_REQUIRED, XML_UTIL_ENTITIES_HTML)
    * @return   string                           string representation of the attributes
    * @uses     XML_Util::replaceEntities() to replace XML entities in attribute values
    * @todo     allow sort also to be an options array
    */
    function attributesToString($attributes, $sort = true, $multiline = false, $indent = '    ', $linebreak = "\n", $entities = XML_UTIL_ENTITIES_XML)
    {
        /**
         * second parameter may be an array
         */
        if (is_array($sort)) {
            if (isset($sort['multiline'])) {
                $multiline = $sort['multiline'];
            }
            if (isset($sort['indent'])) {
                $indent = $sort['indent'];
            }
            if (isset($sort['linebreak'])) {
                $multiline = $sort['linebreak'];
            }
            if (isset($sort['entities'])) {
                $entities = $sort['entities'];
            }
            if (isset($sort['sort'])) {
                $sort = $sort['sort'];
            } else {
                $sort = true;
            }
        }
        $string = '';
        if (is_array($attributes) && !empty($attributes)) {
            if ($sort) {
                ksort($attributes);
            }
            if( !$multiline || count($attributes) == 1) {
                foreach ($attributes as $key => $value) {
                    if ($entities != XML_UTIL_ENTITIES_NONE) {
                        $value = XML_Util::replaceEntities($value, $entities);
                    }
                    $string .= ' '.$key.'="'.$value.'"';
                }
            } else {
                $first = true;
                foreach ($attributes as $key => $value) {
                    if ($entities != XML_UTIL_ENTITIES_NONE) {
                        $value = XML_Util::replaceEntities($value, $entities);
                    }
                    if ($first) {
                        $string .= " ".$key.'="'.$value.'"';
                        $first = false;
                    } else {
                        $string .= $linebreak.$indent.$key.'="'.$value.'"';
                    }
                }
            }
        }
        return $string;
    }

   /**
    * create a tag
    *
    * This method will call XML_Util::createTagFromArray(), which
    * is more flexible.
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // create an XML tag:
    * $tag = XML_Util::createTag("myNs:myTag", array("foo" => "bar"), "This is inside the tag", "http://www.w3c.org/myNs#");
    * </code>
    *
    * @access   public
    * @static
    * @param    string  $qname             qualified tagname (including namespace)
    * @param    array   $attributes        array containg attributes
    * @param    mixed   $content
    * @param    string  $namespaceUri      URI of the namespace
    * @param    integer $replaceEntities   whether to replace XML special chars in content, embedd it in a CData section or none of both
    * @param    boolean $multiline         whether to create a multiline tag where each attribute gets written to a single line
    * @param    string  $indent            string used to indent attributes (_auto indents attributes so they start at the same column)
    * @param    string  $linebreak         string used for linebreaks
    * @param    string  $encoding          encoding that should be used to translate content
    * @return   string  $string            XML tag
    * @see      XML_Util::createTagFromArray()
    * @uses     XML_Util::createTagFromArray() to create the tag
    */
    function createTag($qname, $attributes = array(), $content = null, $namespaceUri = null, $replaceEntities = XML_UTIL_REPLACE_ENTITIES, $multiline = false, $indent = "_auto", $linebreak = "\n", $encoding = XML_UTIL_ENTITIES_XML)
    {
        $tag = array(
                     "qname"      => $qname,
                     "attributes" => $attributes
                    );

        // add tag content
        if ($content !== null) {
            $tag["content"] = $content;
        }
        
        // add namespace Uri
        if ($namespaceUri !== null) {
            $tag["namespaceUri"] = $namespaceUri;
        }

        return XML_Util::createTagFromArray($tag, $replaceEntities, $multiline, $indent, $linebreak, $encoding);
    }

   /**
    * create a tag from an array
    * this method awaits an array in the following format
    * <pre>
    * array(
    *  "qname"        => $qname         // qualified name of the tag
    *  "namespace"    => $namespace     // namespace prefix (optional, if qname is specified or no namespace)
    *  "localpart"    => $localpart,    // local part of the tagname (optional, if qname is specified)
    *  "attributes"   => array(),       // array containing all attributes (optional)
    *  "content"      => $content,      // tag content (optional)
    *  "namespaceUri" => $namespaceUri  // namespaceUri for the given namespace (optional)
    *   )
    * </pre>
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * $tag = array(
    *           "qname"        => "foo:bar",
    *           "namespaceUri" => "http://foo.com",
    *           "attributes"   => array( "key" => "value", "argh" => "fruit&vegetable" ),
    *           "content"      => "I'm inside the tag",
    *            );
    * // creating a tag with qualified name and namespaceUri
    * $string = XML_Util::createTagFromArray($tag);
    * </code>
    *
    * @access   public
    * @static
    * @param    array   $tag               tag definition
    * @param    integer $replaceEntities   whether to replace XML special chars in content, embedd it in a CData section or none of both
    * @param    boolean $multiline         whether to create a multiline tag where each attribute gets written to a single line
    * @param    string  $indent            string used to indent attributes (_auto indents attributes so they start at the same column)
    * @param    string  $linebreak         string used for linebreaks
    * @return   string  $string            XML tag
    * @see      XML_Util::createTag()
    * @uses     XML_Util::attributesToString() to serialize the attributes of the tag
    * @uses     XML_Util::splitQualifiedName() to get local part and namespace of a qualified name
    */
    function createTagFromArray($tag, $replaceEntities = XML_UTIL_REPLACE_ENTITIES, $multiline = false, $indent = "_auto", $linebreak = "\n", $encoding = XML_UTIL_ENTITIES_XML)
    {
        if (isset($tag["content"]) && !is_scalar($tag["content"])) {
            return XML_Util::raiseError( "Supplied non-scalar value as tag content", XML_UTIL_ERROR_NON_SCALAR_CONTENT );
        }

        if (!isset($tag['qname']) && !isset($tag['localPart'])) {
            return XML_Util::raiseError( 'You must either supply a qualified name (qname) or local tag name (localPart).', XML_UTIL_ERROR_NO_TAG_NAME );
        }

        // if no attributes hav been set, use empty attributes
        if (!isset($tag["attributes"]) || !is_array($tag["attributes"])) {
            $tag["attributes"] = array();
        }
        
        // qualified name is not given
        if (!isset($tag["qname"])) {
            // check for namespace
            if (isset($tag["namespace"]) && !empty($tag["namespace"])) {
                $tag["qname"] = $tag["namespace"].":".$tag["localPart"];
            } else {
                $tag["qname"] = $tag["localPart"];
            }
        // namespace URI is set, but no namespace
        } elseif (isset($tag["namespaceUri"]) && !isset($tag["namespace"])) {
            $parts = XML_Util::splitQualifiedName($tag["qname"]);
            $tag["localPart"] = $parts["localPart"];
            if (isset($parts["namespace"])) {
                $tag["namespace"] = $parts["namespace"];
            }
        }

        if (isset($tag["namespaceUri"]) && !empty($tag["namespaceUri"])) {
            // is a namespace given
            if (isset($tag["namespace"]) && !empty($tag["namespace"])) {
                $tag["attributes"]["xmlns:".$tag["namespace"]] = $tag["namespaceUri"];
            } else {
                // define this Uri as the default namespace
                $tag["attributes"]["xmlns"] = $tag["namespaceUri"];
            }
        }

        // check for multiline attributes
        if ($multiline === true) {
            if ($indent === "_auto") {
                $indent = str_repeat(" ", (strlen($tag["qname"])+2));
            }
        }
        
        // create attribute list
        $attList    =   XML_Util::attributesToString($tag["attributes"], true, $multiline, $indent, $linebreak );
        if (!isset($tag["content"]) || (string)$tag["content"] == '') {
            $tag    =   sprintf("<%s%s />", $tag["qname"], $attList);
        } else {
            if ($replaceEntities == XML_UTIL_REPLACE_ENTITIES) {
                $tag["content"] = XML_Util::replaceEntities($tag["content"], $encoding);
            } elseif ($replaceEntities == XML_UTIL_CDATA_SECTION) {
                $tag["content"] = XML_Util::createCDataSection($tag["content"]);
            }
            $tag    =   sprintf("<%s%s>%s</%s>", $tag["qname"], $attList, $tag["content"], $tag["qname"] );
        }        
        return  $tag;
    }

   /**
    * create a start element
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // create an XML start element:
    * $tag = XML_Util::createStartElement("myNs:myTag", array("foo" => "bar") ,"http://www.w3c.org/myNs#");
    * </code>
    *
    * @access   public
    * @static
    * @param    string  $qname             qualified tagname (including namespace)
    * @param    array   $attributes        array containg attributes
    * @param    string  $namespaceUri      URI of the namespace
    * @param    boolean $multiline         whether to create a multiline tag where each attribute gets written to a single line
    * @param    string  $indent            string used to indent attributes (_auto indents attributes so they start at the same column)
    * @param    string  $linebreak         string used for linebreaks
    * @return   string  $string            XML start element
    * @see      XML_Util::createEndElement(), XML_Util::createTag()
    */
    function createStartElement($qname, $attributes = array(), $namespaceUri = null, $multiline = false, $indent = '_auto', $linebreak = "\n")
    {
        // if no attributes hav been set, use empty attributes
        if (!isset($attributes) || !is_array($attributes)) {
            $attributes = array();
        }
        
        if ($namespaceUri != null) {
            $parts = XML_Util::splitQualifiedName($qname);
        }

        // check for multiline attributes
        if ($multiline === true) {
            if ($indent === "_auto") {
                $indent = str_repeat(" ", (strlen($qname)+2));
            }
        }

        if ($namespaceUri != null) {
            // is a namespace given
            if (isset($parts["namespace"]) && !empty($parts["namespace"])) {
                $attributes["xmlns:".$parts["namespace"]] = $namespaceUri;
            } else {
                // define this Uri as the default namespace
                $attributes["xmlns"] = $namespaceUri;
            }
        }

        // create attribute list
        $attList    =   XML_Util::attributesToString($attributes, true, $multiline, $indent, $linebreak);
        $element    =   sprintf("<%s%s>", $qname, $attList);
        return  $element;
    }

   /**
    * create an end element
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // create an XML start element:
    * $tag = XML_Util::createEndElement("myNs:myTag");
    * </code>
    *
    * @access   public
    * @static
    * @param    string  $qname             qualified tagname (including namespace)
    * @return   string  $string            XML end element
    * @see      XML_Util::createStartElement(), XML_Util::createTag()
    */
    function createEndElement($qname)
    {
        $element    =   sprintf("</%s>", $qname);
        return  $element;
    }
    
   /**
    * create an XML comment
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // create an XML start element:
    * $tag = XML_Util::createComment("I am a comment");
    * </code>
    *
    * @access   public
    * @static
    * @param    string  $content           content of the comment
    * @return   string  $comment           XML comment
    */
    function createComment($content)
    {
        $comment    =   sprintf("<!-- %s -->", $content);
        return  $comment;
    }
    
   /**
    * create a CData section
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // create a CData section
    * $tag = XML_Util::createCDataSection("I am content.");
    * </code>
    *
    * @access   public
    * @static
    * @param    string  $data              data of the CData section
    * @return   string  $string            CData section with content
    */
    function createCDataSection($data)
    {
        return  sprintf("<![CDATA[%s]]>", $data);
    }

   /**
    * split qualified name and return namespace and local part
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // split qualified tag
    * $parts = XML_Util::splitQualifiedName("xslt:stylesheet");
    * </code>
    * the returned array will contain two elements:
    * <pre>
    * array(
    *       "namespace" => "xslt",
    *       "localPart" => "stylesheet"
    *      );
    * </pre>
    *
    * @access public
    * @static
    * @param  string    $qname      qualified tag name
    * @param  string    $defaultNs  default namespace (optional)
    * @return array     $parts      array containing namespace and local part
    */
    function splitQualifiedName($qname, $defaultNs = null)
    {
        if (strstr($qname, ':')) {
            $tmp = explode(":", $qname);
            return array(
                          "namespace" => $tmp[0],
                          "localPart" => $tmp[1]
                        );
        }
        return array(
                      "namespace" => $defaultNs,
                      "localPart" => $qname
                    );
    }

   /**
    * check, whether string is valid XML name
    *
    * <p>XML names are used for tagname, attribute names and various
    * other, lesser known entities.</p>
    * <p>An XML name may only consist of alphanumeric characters,
    * dashes, undescores and periods, and has to start with a letter
    * or an underscore.
    * </p>
    *
    * <code>
    * require_once 'XML/Util.php';
    * 
    * // verify tag name
    * $result = XML_Util::isValidName("invalidTag?");
    * if (XML_Util::isError($result)) {
    *    print "Invalid XML name: " . $result->getMessage();
    * }
    * </code>
    *
    * @access  public
    * @static
    * @param   string  $string string that should be checked
    * @return  mixed   $valid  true, if string is a valid XML name, PEAR error otherwise
    * @todo    support for other charsets
    */
    function isValidName($string)
    {
        // check for invalid chars
        if (!preg_match("/^[[:alnum:]_\-.]$/", $string{0})) {
            return XML_Util::raiseError( "XML names may only start with letter or underscore", XML_UTIL_ERROR_INVALID_START );
        }
        
        // check for invalid chars
        if (!preg_match("/^([a-zA-Z_]([a-zA-Z0-9_\-\.]*)?:)?[a-zA-Z_]([a-zA-Z0-9_\-\.]+)?$/", $string)) {
            return XML_Util::raiseError( "XML names may only contain alphanumeric chars, period, hyphen, colon and underscores", XML_UTIL_ERROR_INVALID_CHARS );
         }
        // XML name is valid
        return true;
    }

   /**
    * replacement for XML_Util::raiseError
    *
    * Avoids the necessity to always require
    * PEAR.php
    *
    * @access   public
    * @param    string      error message
    * @param    integer     error code
    * @return   object PEAR_Error
    */
    function raiseError($msg, $code)
    {
        require_once 'PEAR.php';
        return PEAR::raiseError($msg, $code);
    }
}
//} // if (!class_exists('XML_Util'))
?>