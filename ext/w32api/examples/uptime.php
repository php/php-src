<?php
/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: James Moore <jmoore@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/**
 * A little example that registers MessageBoxA and GetTickCount then using
 * these two API functions tells you how long you computer has been
 * running
 */
        /**
         * Define constants needed
         * Taken from Visual Studio/Tools/Winapi/WIN32API.txt
         */
         
        define("MB_OK", 0);

        
        dl("php_w32api.dll");

        w32api_register_function("kernel32.dll", 
                                 "GetTickCount",
                                 "long");
                                 
        w32api_register_function("User32.dll",
                                 "MessageBoxA",
                                 "long");

        $ticks = GetTickCount();

        $secs = floor($ticks / 1000);
        $mins = floor($secs / 60);
        $hours = floor($mins / 60);

        $str = sprintf("You have been using your computer for:".
                        "\r\n %d Milliseconds, or \r\n %d Seconds".
                        "or \r\n %d mins or\r\n %d hours %d mins.",
                        $ticks,
                        $secs,
                        $mins,
                        $hours,
                        $mins - ($hours*60));

        MessageBoxA(NULL, 
                    $str, 
                    "Uptime Information", 
                    MB_OK);
?>
