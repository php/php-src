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
// | Authors: Christian Stocker <chregu@phant.ch>                         |
// +----------------------------------------------------------------------+
//
// $Id$

require_once( "Config/Container.php" ) ;

/**
* Config-API-Implemtentation for DB-Ini-Entries
*
* This class implements the Config-API based on ConfigDefault
*
* The Table structure should be as follows:
*
* CREATE TABLE config (
*   datasrc varchar(50) NOT NULL,
*   block varchar(50) NOT NULL,
*   name varchar(50) NOT NULL,
*   value varchar(50)
* );
*
* You can name the Table other than config, but you have to supply then
*  this name in the $feature->array
*
* @author      Christian Stocker <chregu@phant.ch>
* @access      public
* @version     $Id$
* @package     Config
*/

class Config_Container_db extends Config_Container {


    /**
    * contains the features given by parseInput
    * @var array
    * @see parseInput()
    */
    var $feature = array("Table" => "config") ;

    /**
    * Constructor of the class
    *
    * Connects to the DB via the PEAR::DB-class
    *
    * @param  $dsn string with PEAR::DB "data source name" or object DB object
    */

    function Config_Container_db ($dsn ) {

        //if dsn is a string, then we have to connect to the db
        if (is_string($dsn))
        {
            include_once ("DB.php");
            $this->db = DB::Connect($dsn);
            if (DB::isError($db))
            {
                print "The given dsn was not valid in file ".__FILE__." at line ".__LINE__."<br>\n";
                return new DB_Error($db->code,PEAR_ERROR_DIE);
            }

        }

        //if it's an object, it could be a db_object.
        elseif (is_object($dsn) && DB::isError($dsn))
        {
            print "The given param  was not valid in file ".__FILE__." at line ".__LINE__."<br>\n";
            return new DB_Error($dsn->code,PEAR_ERROR_DIE);
        }

        // if parent class is db_common, then it's already a connected identifier
        elseif (get_parent_class($dsn) == "db_common")
        {
            $this->db = $dsn;
        }

        else {
            return new PEAR_Error( "The given dsn ($dsn) was not valid in file ".__FILE__." at line ".__LINE__, 41, PEAR_ERROR_RETURN, null, null );
        }


    }

    /**
    * parses the input of the given data source
    *
    * The Data Source can be a string with field-name of the datasrc field in the db
    *
    *
    * @access public
    * @param string $datasrc  Name of the datasource to parse
    * @param array $feature   Contains a hash of features
    * @return mixed				returns a DB_ERROR, if error occurs    
    */

    function parseInput( $datasrc = "", $feature = array( ) )
    {
        $this->setFeatures($feature,  array_merge($this->allowed_options,array("Table")));

        $query = "SELECT block,name,value from ".$this->feature["Table"]." where datasrc = '$datasrc'";
        $res = $this->db->query($query);
        if (DB::isError($res))
        {
            return new DB_Error($dsn->code,PEAR_ERROR_DIE);
        }
        else
        {
            while ($entry = $res->FetchRow(DB_FETCHMODE_ASSOC))
            {
                $this->data[$entry[block]][$entry[name]] = $entry[value];
            }
        }


    }


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
    * @return mixed				returns a DB_ERROR, if error occurs        
    * @see parseInput()
    */

    function writeInput( $datasrc = "", $preserve = True )
    {
        $query = "delete from ".$this->feature["Table"]." where datasrc = '$datasrc'";
        $res = $this->db->query($query);
        if (DB::isError($res))
        {
            return new DB_Error($dsn->code,PEAR_ERROR_DIE);
        }


        foreach ($this->data as $block => $blockarray)
        {

            foreach ($blockarray as $name => $value)
            {
                $query = "insert into ".$this->feature["Table"]." (datasrc,block,name,value) VALUES
                         ('".addslashes($datasrc)."','".addslashes($block)."','".addslashes($name)."','".addslashes($value)."')";

                $res = $this->db->query($query);
                if (DB::isError($res))
                {
                    return new DB_Error($dsn->code,PEAR_ERROR_DIE);
                }

            }
        }
    }
}
?>