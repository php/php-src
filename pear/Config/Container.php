<?php
// +---------------------------------------------------------------------+
// | PHP version 4.0                                                     |
// +---------------------------------------------------------------------+
// | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group            |
// +---------------------------------------------------------------------+
// | This source file is subject to version 2.0 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is       |
// | available at through the world-wide-web at                          |
// | http://www.php.net/license/2_02.txt.                                |
// | If you did not receive a copy of the PHP license and areunable to   |
// | obtain it through the world-wide-web, please send a note to         |
// | license@php.net so we can mail you a copy immediately.              |
// +---------------------------------------------------------------------+
// | Authors: Alexander Merz <alexander.merz@t-online.de>                |
// |          Christian Stocker <chregu@phant.ch>                        |
// +---------------------------------------------------------------------+
//
// $Id$

/**
* Interface for Config-Classes
*
* This interface provides a API for configuration data
*
* @author      Alexander Merz <alexander.merz@t-online.de>
* @access      public
* @version     $Id$
* @package     Config
*/



class Config_Container {

    /**
    * parses the input of the given data source
    *
    * The format and required content of $datasrc depends of the implementation.
    * If the implemention requires additional data, for example a comment char, it have to
    * deliver in a hash as second argument.
    *
    * @access public
    * @param string $datasrc  	Name of the datasource to parse
    * @param array $feature   	Contains a hash of features depending on the implentation
    * @return mixed				returns a PEAR_ERROR, if error occurs
    */

    function parseInput( $datasrc = "", $feature = array() )
    {
        return new PEAR_Error("parseInput not implemented", 1, PEAR_ERROR_RETURN, null,
                              null );
    } // end func parseInput

    /**
    * writes the data to the given data source or if not given to the datasource of parseInput
    *
    * See parseInput for $datasrc. If the second argument $preserve is true, the implementation
    * should try to preserve the original format and data of the source except changed or added values.
    * This mean to preserve for example comments in files or so.
    *
    * @access public
    * @param string 	$datasrc    Name of the datasource to parse
    * @param boolean    $preserve   preserving behavior
    * @return mixed					returns PEAR_Error, if fails
    * @see parseInput()
    */

    function writeInput( $datasrc = "", $preserve = true  )
    {
        return new PEAR_Error("writeInput not implemented", 1, PEAR_ERROR_RETURN, null,
                              null );
    } // end func writeInput

    /**
    * returns all key-value-pairs of the given block
    *
    * The implementation should returns only the pairs of the given
    * block, no names of subblocks.
    * Blocks should be looks like this "/rootblock/child_block/grandchild_block/..."
    * Take care of the leading slash - this is a "root"-slash according to the "root"-dir
    * under Unix. If a config data source contains no blocks or using of blocks makes no sense
    * the implementation have to support at least the root-slash.
    *
    * @access public
    * @param string $block   	block path
    * @return array				returns a hash containing all values, but a PEAR_Error if fails
    */

    function getValues( $block )
    {
        return new PEAR_Error("getValues not implemented", 1, PEAR_ERROR_RETURN, null,
                              null );
    } // end func getValues

    /**
    * returns the name of all childblocks
    *
    * The implementation returns the name of all childblocks in the given block as a array.
    * If a childblock contains also childblocks the value of the block have to be also a
    * array containing all grandchild-blocks
    *
    * @access public
    * @param  string $block    	block path
    * @return array				returns a array containing all values, or a PEAR_Error, if fails
    * @see    getValues()
    */

    function getBlocks( $block )
    {
        return new PEAR_Error("getBlocks not implemented", 1, PEAR_ERROR_RETURN, null,
                              null );
    } // end func getBlocks

    /**
    * returns the value of the key in the given block
    *
    * The implementation returns the value of the key in the given block. If the key or the block not exist
    * the implementation should return the default-value and create the block with the key and the default-value.
    *
    * @access public
    * @param  string $block    block path
    * @param  string $key      key
    * @param  string $default  default-value
    * @return mixed			   type depends of the value
    * @see    getValues()
    */

    function getValue( $block, $key, $default = "" )
    {
        return new PEAR_Error("getValue not implemented", 1, PEAR_ERROR_RETURN, null,
                              null );
    } // end func getValue

    /**
    * sets the value of the key in the given block
    *
    * The implementation sets the value of the key in the given block and returns value.
    * If the key or the block not exist, the implementation should create the block with
    * the key and the value.
    *
    * @access public
    * @param  string $block    	block path
    * @param  string $key      	key
    * @param  string $value    	value to set
    * @return mixed				type depends on $value
    * @see    getValue()
    */

    function setValue( $block, $key, $value )
    {
        return new PEAR_Error("setValue not implemented", 1, PEAR_ERROR_RETURN, null,
                              null );
    } // end func setValue

    /**
    * Imports the requested datafields as object variables if allowed
    *
    * @param    array   List of fields to be imported as object variables
    * @param    array   List of allowed datafields
    */

    function setFeatures($requested, $allowed) {
        foreach ($allowed as $k => $field)
        if (isset($requested[$field]))
            $this->feature[$field] = $requested[$field];

    } // end func setFeatures
}

?>
