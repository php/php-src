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
// +----------------------------------------------------------------------+
//
// $Id$

require_once( "Config/Container.php" ) ;

/**
* Config-API-Implemtentation for Ini-Files
*
* This class implements the Config-API based on ConfigDefault
*
* @author      Alexander Merz <alexander.merz@t-online.de>
* @access      public
* @version     $Id$
* @package     Config
*/

class Config_Container_IniFile extends Config_Container {

/**
* contains the features given by parseInput
* @var array
* @see parseInput()
*/
var $feature = array() ;

/**
* parses the input of the given data source
*
* The Data Source can be a string with the file or a array of strings with the files to read,
* so datasrc requires a existing file.
* The feature-array have to contain the comment char array("cc" => Comment char)
*
* @access public
* @param string $datasrc  Name of the datasource to parse
* @param array $feature   Contains a hash of features
* @return mixed				returns a PEAR_ERROR, if error occurs
*/

function parseInput( $datasrc = "", $feature = array( "cc" => ";") )
{

    // Checking if $datasrc is a array, then call parseInput with
    // each file
    if( is_array( $datasrc) ) {
         foreach( $datasrc as $file ) {
                  $ret = $this -> parseInput( $file, $feature ) ;
                  if( PEAR::isError( $ret) ) {
                      return $ret ;
                  }
         }
    }


    $this -> datasrc = $datasrc ;
    $this -> feature = $feature ;
    if( file_exists( $datasrc ) )
    {
        $lines = file( $datasrc ) ;
        $block = "/" ;
        $zeilennr = 0 ;
        foreach( $lines as $line)
        {
                 $value_found = "" ;
                 $kkey_found = "" ;

                 $line = trim( $line) ;
                 // is it a blank line?
                 if( empty( $line) )
                 {
                     $found = 0 ;
                 }
                 // checking for data
                 else
                 {
                     $char = substr( $line, 0, 1) ;

                     // a comment?
                     if( $char == $this -> feature["cc"] )
                     {
                         $found = 0 ;
                     }
                     // a block?
                     elseif( $char == '[' )
                     {
                         $found = 1 ;
                         $block_found = substr( $line, 1, strpos( substr( $line, 1 ), ']' ) ) ;
                     }
                     // must be a kvp
                     else
                     {
                         $key = substr( $line, 0, strpos( $line, '=') ) ;
                         if( !$key )
                         {
                             return new PEAR_Error( "Line ".$zeilennr." in '".$datasrc."' seems to be a comment, but comment char is missing!", 41, PEAR_ERROR_RETURN, null, null );
                         }
                         $value = (string)trim( (string)substr( (string)$line, strpos( (string)$line, '=') ) ) ;
                         $value = (string)trim( (string)substr( (string)$value, 1 ) );

                         // checking for double quoted string
                         if( '"' == substr( $value, 0, 1) )
                         {
                             // value = all until next "
                             $value_found = substr( $value, 0, strpos( substr( $value, 1 ), '"' ) + 1) ;
                             $value_found = substr( $value_found, 1, strlen( $value_found) ) ;
                         }
                         else
                         {
                            // value = all until next space, eol or comment
                            preg_match( "/([^\s".$this -> feature["cc"]."]*)[\s]*[".$this -> feature["cc"]."]*[\s]*[\S]*[\s]*/", trim( $value ), $match) ;
                            $value_found = $match[1] ;
                         }

                         $key_found = trim( $key ) ;
                         $found = 2 ;
                     }
                 }

                 // creating the array
                 switch( $found )
                 {
                     case 1 :
                            $block = '/'.$block_found ;
                          break ;
                     case 2 :

                            $aadd = $this -> data[ $block ] ;
                            $aadd[ $key_found ] = (string)trim ((string)$value_found ) ;
                            $this -> data[$block] = $aadd ;
                          break ;
                     case 0 :
                     default :
                          break ;
                 }
          $zeilennr++ ;
        }
    }
    else
    {
        return new PEAR_Error( "File '".$datasrc."' doesn't exists!", 31, PEAR_ERROR_RETURN, null, null );
    }

} // end func parseInput

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
* @return object PEAR_Error
* @see parseInput()
*/

function writeInput( $datasrc = "", $preserve = true  )
{

    if( empty( $datasrc ) ) {
        $datasrc = $this -> datasrc ;
    }
    elseif( !file_exists( $datasrc )) {
        return new PEAR_Error("File '$datasrc' doesn't exist", 41, PEAR_ERROR_RETURN, null,
                              null );
    }

    if( $preserve ) {
        return new PEAR_Error("Preserving not supported", 49, PEAR_ERROR_RETURN, null,
                              null );
    }
    else {
        $fh = fopen( $datasrc, "w") ;
        if( !$fh) {
             return new PEAR_Error("Couldn't open '$datasrc' for writing", 42, PEAR_ERROR_RETURN, null,
                              null );
        }
        else {
            foreach( $this -> data as $block => $blockkv ) {
                $block = substr( $block, 1) ;
                fwrite( $fh, "[$block]\n" ) ;
                foreach( $blockkv as $key => $value ) {
                         if( strpos ( $value, ' ' )) {
                             $value = '"'.$value.'"' ;
                         }
                         fwrite( $fh, "$key = $value\n" ) ;
                }
                fwrite( $fh, "\n" ) ;
            }
            fclose( $fh) ;
        }

    }
} // end func writeInput


}; // end class Config_IniFile



?>
