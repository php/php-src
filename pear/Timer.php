<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Sebastian Bergmann <sb@phpOpenTracker.de>                   |
// |          (based upon code by Allan Kent <allan@itsolutions.co.za>)   |
// +----------------------------------------------------------------------+
//
// $Id$
//

  /**
  * PEAR/Timer
  * 
  * Purpose:
  * 
  *   Timing Script Execution, Generating Profiling Information
  * 
  * Example:
  * 
  *   $timer = new Timer;
  * 
  *   $timer->start();
  *   $timer->set_marker( "Marker 1" );
  *   $timer->stop();
  * 
  *   $timer->profiling_information();
  * 
  * @author	  Sebastian Bergmann <sb@phpOpenTracker.de>
  * @version  1.0 28/09/00
  * @access		public
  */

  class Timer
  {
    // {{{ properties

  	/**
  	* Contains the markers
  	* @var		array
  	* @access	public
  	*/

    var $markers = array();
    
    // }}}
    // {{{ start()

  	/**
  	* Set "Start" marker.
  	* @brother stop()
  	* @access	public
  	*/

    function start()
    {
      $this->set_marker( "Start" );
    }

    // }}}
    // {{{ stop()

  	/**
  	* Set "Stop" marker.
  	* @brother start()
  	* @access	public
  	*/

    function stop()
    {
      $this->set_marker( "Stop" );
    }

    // }}}
    // {{{ set_marker()

  	/**
  	* Set marker.
  	* @brother stop()
  	* @access	public
  	*/

    function set_marker( $name )
    {
      $microtime = explode( " ", microtime() );
      $this->markers[ $name ] = $microtime[ 1 ] . substr( $microtime[ 0 ], 1 );
    }

    // }}}
    // {{{ time_elapsed()

  	/**
  	* Returns the time elapsed betweens two markers.
    * @param  string  $start        start marker, defaults to "Start"
    * @param  string  $end          end marker, defaults to "Stop"
    * @return double  $time_elapsed time elapsed between $start and $end
  	* @access	public
  	*/

    function time_elapsed( $start = "Start", $end = "Stop" )
    {
      return bcsub( $this->markers[ $end ], $this->markers[ $start ], 6 );
    }
    
    // }}}
    // {{{ profiling_information()

  	/**
  	* Prints profiling information.
  	* @access	public
  	*/

    function profiling_information()
    {
      print "<table border=\"0\" cellSpacing=\"3\" cellPadding=\"3\"><tr><td><b>Marker</b></td><td><b>Time</b></td><td><b>Diff</b></td></tr>";

      while( list( $marker, $time ) = each( $this->markers ) )
      {
        print "<tr><td>" . $marker . "</td>";
        print "<td>" . $time . "</td>";

        if( $marker == "Start" )
        {
          $diff = "-";
        }

        else
        {
          $diff = bcsub( $time, $temp, 6 );
        }

        print "<td>" . $diff . "</td></tr>";

        $temp = $time;
      }

      print "<tr><td>&nbsp;</td><td align=\"right\">Total:</td><td>" . $this->time_elapsed() . "</td></tr></table>";
    }

    // }}}
  }
?>
