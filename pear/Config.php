<?php
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Alexander Merz <alexander.merz@t-online.de>                 |
// |          Christian Stocker <chregu@phant.ch>                         |
// +----------------------------------------------------------------------+
//
// $Id$

require_once( "PEAR.php") ;

/**
* Partly implementation of the Config-Interface-API
*
* This class implements a part of the API for working on configuration data
* ConfigDefault implements getValues(), getBlocks(), getValue(), setValue()
* The data is internally saved in a nested array.
*
* Example:
*   include("Config.php");
*   $i = new Config("IniFile");
*   $i -> parseInput( "yourfile.ini");
*   $ret = $i->getValues('/section');
*
*
* @author      Alexander Merz <alexander.merz@t-online.de>
* @access      public
* @version     $Id$
* @package     Config
*/

class Config  {

    /**
    * contains the data source given by parseInput
    * @var string
    */
    var $datasrc ;

    /**
    * contains the internal data structure
    * @var array
    */
    var $data = array() ;

	/**
	*	Constructor
	*
	*	requires the type of the data container, if the container needs
	*	special parameters during creation, set them with $storage_options
	*
	*	@access public
	*	@param	string	$storage_driver 	type of container
	*	@param	string	$storage_options	parameter for container constructor
	*/
    function Config($storage_driver,$storage_options = "")
    {

        $storage_class = 'Config_Container_' . $storage_driver;
        $storage_classfile = 'Config/Container/' . $storage_driver . '.php';

        include_once $storage_classfile;
        $this->container = new $storage_class($storage_options);
    } // end func Config() 



    /**
    * returns all key-value-pairs of the given block
    *
    * If the block not exists, a PEAR_Error will returned, else
    * a hash: $array["key"] = value
    *
    * @access public
    * @param string $block    block path
    * @return array				returns a hash containing all values, but a PEAR_Error if fails
    */

    function getValues( $block = "/")
    {
        if( !empty( $this -> data ) )
        {
            // if leading slash was forgotten...
            if( "/" != substr( $block, 0, 1) )
            {
                $block = "/".$block ;
            }
            if( isset( $this -> data[ $block ] ) )
            {
                if( is_array( $this -> data[ $block ] ) )
                {
                    $ret = $this -> data[ $block ] ;
                }
                else
                {
                    $ret = false ;
                }
            }
            else
            {
                $ret = new PEAR_Error("Block path '".$block."' doesn't exists! Request couldn't be answered.", 12, PEAR_ERROR_RETURN, null, null );
            }
        }
        else
        {
            $ret = new PEAR_Error("No internal data! Request couldn't be answered.", 11, PEAR_ERROR_RETURN, null, null );
        }

        return $ret ;
    } // end func getValues

    /**
    * returns all blocks of the given block
    *
    * If the block not exists, a PEAR_Error will returned, else
    * a array containing all child blocks
    *
    * @access public
    * @param string $block    block path
    * @return array				returns a array containing all values, or a PEAR_Error, if fails
    */

    function getBlocks( $block = "/")
    {
        if( !empty( $this -> data ) )
        {
            // if leading slash was forgotten...
            if( "/" != substr( $block, 0, 1) )
            {
                $block = "/".$block ;
            }
            $ret = array() ;
            foreach( $this -> data as $key => $value)
            {
                $key = $key."/" ;
                if( $block == substr( $key, 0, strlen( $block ) ) )
                {
                    array_push ( $ret, trim( substr( $key, strlen( $block ), strpos( substr( $key, strlen( $block ) ), "/" ) ) ) ) ;
                }
            }
        }
        else
        {
            $ret = new PEAR_Error("No internal data! Request couldn't be answered.", 21, PEAR_ERROR_RETURN, null, null );
        }

        return $ret ;
    } // end func getBlocks

    /**
    * sets the value of the key of the given block
    *
    * If the block or the key not exists, both will be created.
    * The value will be returned.
    *
    * @access public
    * @param string $block    block path
    * @param string $key      key to set
    * @param string $value    value for the key
	* @return mixed				type depends on $value
	* @see getValue()
    */

    function setValue( $block = "/", $key, $value = "")
    {
        // if leading slash was forgotten...
        if( "/" != substr( $block, 0, 1) )
        {
            $block = "/".$block ;
        }
        // check for existing block and key
        if( !isset ( $this -> data[ $block ] ) )
        {
            $this->data[ $block ] = array() ;
        }
        $kvp = $this -> data[ $block ] ;
        $kvp[ $key ] = $value ;
        $this -> data[ $block ] = $kvp ;
        $ret = $value ;

        return $ret ;
    } // end func setValue

    /**
    * return the value of the key of the given block
    *
    * If the block or the key not exists, both will be created and
    * sets on the default.
    * The value or if not exists the default will be returned.
    *
    * @access public
    * @param string $block    block path
    * @param string $key      key to set
    * @param string $default  default value for the key
    * @return mixed			   type depends of the value
    */

    function getValue( $block = "/", $key, $default = "")
    {
        // if leading slash was forgotten...
        if( "/" != substr( $block, 0, 1) )
        {
            $block = "/".$block ;
        }
        // check for existing block and key
        $values = $this -> getValues( $block ) ;
        if( PEAR::isError($values) or !in_array( $key, array_keys( $values) )  )
        {
            $this -> setValue( $block, $key, $default) ;
            $values = $this -> getValues( $block ) ;
        }
        $ret = $values[ $key ] ;

        return $ret ;
    } // end func getValue

    /**
    * parses the input of the given data source
    *
    * The format and required content of $datasrc depends of the implementation.
    * If the implemention requires additional data, for example a comment char, it have to
    * deliver in a hash as second argument.
    *
    * @access public
    * @param string $files  	Name of the datasource to parse
    * @param array $feature   	Contains a hash of features depending on the implentation
    * @return mixed				returns a PEAR_ERROR, if error occurs
    */

    function parseInput ($files ,$feature = array() )
    {
        if (is_array($files)) {
            $totaldata = array();
            foreach ($files as $datasrc)
            {
                $this->container->parseInput($datasrc,$feature);
                $totaldata = $this->array_merge_clobber($totaldata,$this->container->data);
                unset ($this->data);
                $this->datasrc = $datasrc;
            }
            $this->data = $totaldata;
        }
        else
        {
            $this->container->parseInput($files,$feature);
            $this->data = $this->container->data;
            $this->datasrc = $files;
        }

    } // end func parseInput()
    
    
    /**
    * writes the data to the given data source or if not given to the datasource of parseInput
    * If $datasrc was a array, the last file will used.
    *
    * See parseInput for $datasrc. If the second argument $preserve is true, the implementation
    * should try to preserve the original format and data of the source except changed or added values.
    * This mean to preserve for example comments in files or so.
    *
    * @access public
    * @param string $datasrc    Name of the datasource to parse
    * @param boolean    $preserve   preserving behavior
    * @return mixed					returns PEAR_Error, if fails
    * @see parseInput()
    */
    function writeInput( $datasrc = "", $preserve = True )
    {
        if( empty( $datasrc ) ) {
            $datasrc = $this -> datasrc ;
        }
        $this->container->writeInput($datasrc,$preserve);
    }



        //taken from kc@hireability.com at http://www.php.net/manual/en/function.array-merge-recursive.php
    /**
    * There seemed to be no built in function that would merge two arrays recursively and clobber
    *   any existing key/value pairs. Array_Merge() is not recursive, and array_merge_recursive
    *   seemed to give unsatisfactory results... it would append duplicate key/values.
    *
    *   So here's a cross between array_merge and array_merge_recursive
    *
    * @param    array first array to be merged
    * @param    array second array to be merged
    * @return   array merged array
    * @acces private
    */
    function array_merge_clobber($a1,$a2)
    {
        if(!is_array($a1) || !is_array($a2)) return false;
        $newarray = $a1;
        while (list($key, $val) = each($a2))
        {
            if (is_array($val) && is_array($newarray[$key]))
            {
                $newarray[$key] = $this->array_merge_clobber($newarray[$key], $val);
            }
            else
            {
                $newarray[$key] = $val;
            }
        }
        return $newarray;
    }



}; // end class Config







?>
