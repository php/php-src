<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Shane Caraveo <Shane@Caraveo.com>                           |
// +----------------------------------------------------------------------+
//
// $Id$
//

require_once 'SOAP/Server.php';

$server = new SOAP_Server;

require_once 'server_round2_base.php';
require_once 'server_round2_groupB.php';
require_once 'server_round2_groupC.php';

$server->service(isset($HTTP_RAW_POST_DATA)?$HTTP_RAW_POST_DATA:NULL);
#echo "Content-Length is {$_SERVER['CONTENT_LENGTH']}\n\n";
#echo "<!---\n\nlen:".strlen($HTTP_RAW_POST_DATA)."\n\n";
#echo "[$HTTP_RAW_POST_DATA";
#print "]\n\nend of input data-->";
?>