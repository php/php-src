<?php
/* vim: set expandtab tabstop=4 shiftwidth=4; */
// +---------------------------------------------------------------------+
// |  PHP version 4.0                                                    |
// +---------------------------------------------------------------------+
// |  Copyright (c) 1997-2002 The PHP Group                              |
// +---------------------------------------------------------------------+
// |  This source file is subject to version 2.0 of the PHP license,     |
// |  that is bundled with this package in the file LICENSE, and is      |
// |  available through the world-wide-web at                            |
// |  http://www.php.net/license/2_02.txt.                               |
// |  If you did not receive a copy of the PHP license and are unable to |
// |  obtain it through the world-wide-web, please send a note to        |
// |  license@php.net so we can mail you a copy immediately.             |
// +---------------------------------------------------------------------+
// |  Authors:  Christian Stocker <chregu@phant.ch>                      |
// +---------------------------------------------------------------------+


/*

    WARNING: Due to internal changes in PEAR, the webinstaller is
             currently not supported!

*/


/* This class should simplify the task of installing PEAR-packages, if you
 *  don't have a cgi-php binary on your system or you don't have access to
 *  the system-wide pear directory.
 *
 *  To use it, make the following php-script:
 *
 *  <?php
 *      require("PEAR/WebInstaller.php");
 *      $installer = new PEAR_WebInstaller("/path/to/your/install/dir","http://php.chregu.tv/pear/");
 *      $installer->start();
 *  ?>
 *
 *  and put PEAR/WebInstaller.php (this script) anywhere in your include_path.
 *
 *  (http://php.chregu.tv/pear/ is just for testing purposes until this
 *    system runs on pear.php.net, but feel free to use it till then)
 *
 *  Both parameters are optional. If the install dir is ommitted, the
 *  installer takes either the system wide pear-directory (mostly
 *  /usr/local/lib/php on unix), if it's writeable, or else the directory
 *  the script is started. Be advised, that the directory, where the
 *  PEAR::Packages will be installed, has to be writeable for the web-server.
 *
 *  The second parameter points to the server/directory where all the
 *  packages and especially Packages.xml is located. If not given, the
 *  standard PEAR-Repository is taken (http://pear.php.net/whatever..)
 *
 *  After installation, just add the install-dir to your include_path and
 *  the packages should work.
 *
 *  If you are System Adminisitrator and want the installed packages to be
 *  made available for everyone, just copy the files to the systemwide
 *  pear-dir after installation on the commandline. Or also add the
 *  install-dir to the systemwide include_path (and maybe don't forget to
 *  take the writeable off the directory..)
 *
 *  TODO:
 *      - More Error Detection
 *      - Grouping of Packages
 *      - Show installed Packages (from /var/lib/php/packages.lst?)
 *      - Add possibility to install a package (.tgz) from the local file
 *           system without a global Packages.xml (useful if no cgi-php
 *           around and you need this package you downloaded installed :) )
 *      - Search Function (maybe needed if we have hundreds of packages)
 *      - Only download Packages.xml, if it actually changed.
 *
 *  This Code is highly experimental.
 */

require_once "PEAR.php";

class PEAR_WebInstaller extends PEAR
{
    // {{{ properties

    /** stack of elements, gives some sort of XML context */
    var $element_stack;

    /** name of currently parsed XML element */
    var $current_element;

    /** array of attributes of the currently parsed XML element */
    var $current_attributes = array();

    /** assoc with information about a package */
    var $pkginfo = array();

    /** assoc with information about all packages */
    var $AllPackages;

    /** URL to the server containing all packages in tgz-Format and the Package.xml */
    var $remotedir = "http://php.chregu.tv/pear/";

    /*  Directory where the to be installed files should be put
        per default PEAR_INSTALL_DIR (/usr/local/lib/php) if it's writeable for the webserver,
        else current directory, or user defined directory (provided as first parameter in constructor)
        The Directory hast to be writeable for the php-module (webserver)
    */
    var $installdir;

    /** how many seconds we should cache Packages.xml */
    var $cachetime = 3600;

    var $printlogger = True;
    // }}}

    // {{{ constructor

    function PEAR_Webinstaller($installdir = Null,$remotedir = Null)
    {
        $this->PEAR();
        if ($installdir)
        {
            $this->installdir = $installdir;
        }
        else
        {
            if (is_writeable(PEAR_INSTALL_DIR))
            {
                $this->installdir = PEAR_INSTALL_DIR;
            }
            else
            {
                $this->installdir = getcwd();
            }
        }

        if ($remotedir)
        {
            $this->remotedir = $remotedir;
        }
    }

    // }}}
    // {{{ start()

    function start() {
        global $HTTP_POST_VARS,$HTTP_GET_VARS;

        //print header
        $this->header();

        $this->loggerStart();

        //if some checkboxes for installation were selected, install.
        if ($HTTP_GET_VARS["help"]) {

            $this->help($HTTP_GET_VARS["help"]);
        }

        elseif ($HTTP_POST_VARS["InstPack"]) {
            $this->installPackages(array_keys($HTTP_POST_VARS["InstPack"]));
        }

        //else print all modules
        else {
            $this->printTable();
        }
        $this->footer();
    }
    // }}}
    // {{{ installPackages()

    /* installs the Packages and prints if successfull or not */

    function installPackages($packages)
    {
        require_once "PEAR/Installer.php";
        $installer =& new PEAR_Installer();
        $installer->phpdir = $this->installdir;
        $this->loggerEnd();
        print "<TABLE CELLSPACING=0 BORDER=0 CELLPADDING=1>";
        print "<TR><TD BGCOLOR=\"#000000\">\n";
        print "<TABLE CELLSPACING=1 BORDER=0 CELLPADDING=3 width=100%>\n";
        print " <TR BGCOLOR=\"#e0e0e0\">\n";
        print "  <TH>Package</TH>\n";
        print "  <TH>Status</TH>\n";

        foreach ($packages as $package)
        {


            if (++$i % 2) {
                $bg1 = "#ffffff";
                $bg2 = "#f0f0f0";
            }
            else {
                $bg1 = "#f0f0f0";
                $bg2 = "#e0e0e0";
            }
            print " <TR>\n";

            print "<TD BGCOLOR=\"$bg1\">";
            print $package;
            print "</TD>\n";


            /*
            print "<TD BGCOLOR=\"$bg2\">";
            print "Installing ...";
            print "</td>";
            print " <TR>\n";
            print "<TD BGCOLOR=\"$bg1\">";
            print "&nbsp;";
            print "</TD>\n";
            */
            print "<TD BGCOLOR=\"$bg2\">";
            if (PEAR::isError($installer->Install($this->remotedir."/".$package.".tgz"))) {
                print "\ninstall failed\n";
            }
            else {

                print "install ok\n";
            }
            print "</td></tr>\n";
        }
        print "</td></tr>";
        print "</table>";
        print "<TABLE CELLSPACING=1 BORDER=0 CELLPADDING=3 width=\"100%\">\n";
        print " <TR BGCOLOR=\"$bg1\">\n";
        print "<th colspan=\"2\">";
        print "<a href=\"$GLOBALS[PHP_SELF]\">Back to the Packages</a>\n";
        print "</th></tr></table>";
        print"</td></tr></table>";
    }

    // }}}
    // {{{ printTable()
    /*  Prints a table with all modules available on the server-directory */

    function printTable()
    {
    global $PHP_SELF;
        $Packages = $this->getPackages();
        if (PEAR::IsError($Packages))
        {
            if ($this->printlogger) {
                $this->logger($Packages->message);
            }
            else
            {
                print $Packages->message;
            }
            return $Packages;
        }
        $this->loggerEnd();
        print "<FORM action=\"$GLOBALS[PHP_SELF]\" method=\"post\">\n";
        print "<TABLE CELLSPACING=0 BORDER=0 CELLPADDING=1>";
        print "<TR><TD BGCOLOR=\"#000000\">\n";
        print "<TABLE CELLSPACING=1 BORDER=0 CELLPADDING=3 width=\"100%\">\n";
        print "<tr bgcolor=\"f0f0f0\">";
        print "<td COLSPAN=\"6\" ><input type=\"submit\" value=\"Install\"></td>";
        print "</tr>";

        print " <TR BGCOLOR=\"#e0e0e0\" >\n";
        print "  <TH>Inst.</TH>\n";
        print "  <TH>Package</TH>\n";
        print "  <TH>Summary</TH>\n";
        print "  <TH>Version</TH>\n";
        print "  <TH>Release date</TH>\n";
        print "  <TH>Release notes</TH>\n";
        print " </TR>\n";
        $i = 0;

        ksort($Packages);
        foreach ( $Packages as $package) {

            if (++$i % 2) {
                $bg1 = "#ffffff";
                $bg2 = "#f0f0f0";
            }
            else {
                $bg1 = "#f0f0f0";
                $bg2 = "#e0e0e0";
            }


            print "<TR>\n";

            print "<TD align=\"middle\" BGCOLOR=\"$bg2\">";
            print "<input type=\"checkbox\" name=\"InstPack[".$package["name"]."-".$package["version"]."]\">\n";
            print "</TD>\n";

            print "  <TD BGCOLOR=\"$bg1\">";
            print $this->printCell ($package["name"],"http://pear.php.net/pkginfo.php?package=$package[name]");
            print "</TD>\n";

            print "<TD BGCOLOR=\"$bg2\">";
            $this->printCell ($package["summary"]);
            print "</TD>\n";

            print "<TD BGCOLOR=\"$bg2\">";
            $this->printCell ($package["version"],$this->remotedir."/".$package["name"]."-".$package["version"].".tgz");
            print "</TD>\n";

            print "<TD BGCOLOR=\"$bg2\">";
            $this->printCell ($package["release_date"]);
            print "</TD>\n";

            print "<TD BGCOLOR=\"$bg2\">";
            $this->printCell (nl2br($package["release_notes"]));
            print "</TD>\n";
            print " </TR>\n";

        }
        print "<tr bgcolor=\"$bg1\">";
        print "<td COLSPAN=\"6\" ><input type=\"submit\" value=\"Install\"></td>";
        print "</tr>";
        print "</TABLE> \n";


        print "<TABLE CELLSPACING=1 BORDER=0 CELLPADDING=3 width=\"100%\">\n";
        print " <TR BGCOLOR=\"#e0e0e0\">\n";

        print "<th align=left width=\"10%\" nowrap>\n";
        print "Install Directory: </th><td>$this->installdir";
        if (!is_writable($this->installdir))
        {
            print " <font color=\"red\">(Directory is NOT writeable!)</font>";
        }

        print "</td></tr>\n";

        print " <TR BGCOLOR=\"#f0f0f0\">\n";
        print "<th align=left width=\"10%\" nowrap>\n";
        print "PEAR Repository: </th><td>$this->remotedir</td></tr>\n";

        print " <TR BGCOLOR=\"#e0e0e0\">\n";
        print "<th align=left width=\"10%\" nowrap>\n";
        print "Caching Time: </th><td>$this->cachetime seconds</td></tr>\n";

        print "</table>\n";
        print "</tr></td></table></FORM>\n";
        print "<a href=\"$PHP_SELF?help=1\">Help</A>\n";
    }

    // }}}
    // {{{ getPackages()

    /** gets the Packages.xml from the server and saves it on the local disc for caching (if possible)
      * If the zlib-extension is compiled in, Packages.xml.gz is used instead.
      */

    function getPackages ($TryGz = True)
    {

        // if we can write to the installdir, cache the Packages.xml there

        $PackageFile = "Packages.xml";

        // check if we have the zlib-extension compiled in
        if ($TryGz && function_exists("gzfile")) { $useGz = True; $PackageFile .= ".gz";}

        // check if we can write the Package.xml file for caching

        if ( (file_exists($this->installdir."/$PackageFile") && is_writeable($this->installdir."/$PackageFile")) || !file_exists($this->installdir."/$PackageFile") && is_writeable($this->installdir) )
        {
            $time = filemtime($this->installdir."/$PackageFile");

            if ($time < (time () - $this->cachetime )) {
                $this->logger("$PackageFile too old. Get new one.");
                $fp =  @fopen($this->remotedir."/$PackageFile","r");
                if (!$fp) {
                    if ($useGz)
                    {
                        $this->logger("$PackageFile could not be read. Try uncompressed one");
                        return $this->getPackages(False);
                    }
                    else {
                        $this->logger("$PackageFile could not be read.");
                        return $this->raiseError("$PackageFile could not be read.");
                    }
                }
                $fout = fopen($this->installdir."/$PackageFile","w");
                while ($data = fread($fp,8192)) {
                    fwrite ($fout, $data);
                }
                fclose($fout);
                fclose($fp);
                $this->logger("Got $PackageFile");
            }
            else {
                $this->logger("Cached $PackageFile seems new enough");
            }
            $Packages = $this->infoFromDescriptionFile($this->installdir."/$PackageFile");
        }
        else
        {
            $this->logger("$PackageFile can not be cached, because Install-Dir or $PackageFile is not writeable. Get it each time from the server");
            $Packages = $this->infoFromDescriptionFile($this->remotedir."/Packages.xml");
        }
        $this->logger("Got Packages");
        return $Packages;
    }

    // }}}
    // {{{ printCell()

    function printCell($text,$link = Null)
    {
        if ($text)
        {
            if ($link) {
                print "<a href=\"$link\" style=\"color: #000000;\">";
            }

            print "$text";

            if ($link) {
                print "</a>";
            }

        }
        else
        {
            print "&nbsp;";
        }
    }

    // }}}
    /* The following 4 functions are taken from PEAR/Common.php written by Stig Bakken
        I had to adjust to use the Packages.xml format.
    */
    // {{{ _element_start()


    function _element_start($xp, $name, $attribs)
    {
        array_push($this->element_stack, $name);
        $this->current_element = $name;
        $this->current_attributes = $attribs;
    }

    // }}}
    // {{{ _element_end()

    function _element_end($xp, $name)
    {
        array_pop($this->element_stack);
        if ($name == "PACKAGE")
        {
            $this->AllPackages[$this->pkginfo["name"]] = $this->pkginfo;
            $this->pkginfo = array();

        }

        $this->current_element = $this->element_stack[sizeof($this->element_stack)-1];
    }

    // }}}
    // {{{ _pkginfo_cdata()

    function _pkginfo_cdata($xp, $data)
    {
        $next = $this->element_stack[sizeof($this->element_stack)-1];
        switch ($this->current_element) {
        case "NAME":
                $this->pkginfo["name"] .= $data;
            break;
        case "SUMMARY":
            $this->pkginfo["summary"] .= $data;
            break;
        case "USER":
            $this->pkginfo["maintainer_handle"] .= $data;
            break;
        case "EMAIL":
            $this->pkginfo["maintainer_email"] .= $data;
            break;
        case "VERSION":
            $this->pkginfo["version"] .= $data;
            break;
        case "DATE":
            $this->pkginfo["release_date"] .= $data;
            break;
        case "NOTES":
            $this->pkginfo["release_notes"] .= $data;
            break;
        case "DIR":
            if (!$this->installdir) {
                break;
            }
            $dir = trim($data);
            // XXX add to file list
            break;
        case "FILE":
            $role = strtolower($this->current_attributes["ROLE"]);
            $file = trim($data);
            // XXX add to file list
            break;
        }
    }

    // }}}
    // {{{ infoFromDescriptionFile()

    function infoFromDescriptionFile($descfile)
    {
        $fp = @fopen($descfile,"r");
        if (!$fp) {
            return  $this->raiseError("Unable to open $descfile in ".__FILE__.":".__LINE__);
        }
        $xp = @xml_parser_create();

        if (!$xp) {
            return $this->raiseError("Unable to create XML parser.");
        }

        xml_set_object($xp, $this);

        xml_set_element_handler($xp, "_element_start", "_element_end");
        xml_set_character_data_handler($xp, "_pkginfo_cdata");
        xml_parser_set_option($xp, XML_OPTION_CASE_FOLDING, true);

        $this->element_stack = array();
        $this->pkginfo = array();
        $this->current_element = false;
        $this->destdir = '';

        // read the whole thing so we only get one cdata callback
        // for each block of cdata

        if (preg_match("/\.gz$/",$descfile))
        {
            $data = implode("",gzfile($descfile));
        }
        else
        {
            $data = implode("",file($descfile));
        }

        if (!@xml_parse($xp, $data, 1)) {
            $msg = sprintf("XML error: %s at line %d",
                           xml_error_string(xml_get_error_code($xp)),
                           xml_get_current_line_number($xp));
            xml_parser_free($xp);
            return $this->raiseError($msg);
        }

        xml_parser_free($xp);

        foreach ($this->pkginfo as $k => $v) {
            $this->pkginfo[$k] = trim($v);
        }

        return $this->AllPackages;
    }

    // }}}
    // {{{ header()

    function header ()
    {
        print "<html>
        <head>
        <title>PEAR::WebInstaller</title>\n";
        if (file_exists("./style.css"))
        {
            print '<link rel="stylesheet" href="/style.css">';
        }
        print "</head>
        <body bgcolor=\"#FFFFFF\">
        <h3>PEAR::WebInstaller</h3>";

    }

    // }}}
    // {{{ footer()

    function footer () {
        print "</body></html>";
    }

    // }}}

    function logger ($text) {

        if ($this->printlogger) {
            if (++$this->logcol % 2) {
                $bg1 = "#ffffff";
                $bg2 = "#f0f0f0";
            }
            else {
                $bg1 = "#f0f0f0";
                $bg2 = "#e0e0e0";
            }
            print "<TR>\n";
            print "<TD BGCOLOR=\"$bg1\">".date("h:m:i",time())."</td>";
            print "<TD BGCOLOR=\"$bg2\">";
            print "$text\n";
            print "</TD>\n";
            print "</tr>";
        }
    }
    function loggerStart () {
        if ($this->printlogger) {
            print "<TABLE CELLSPACING=0 BORDER=0 CELLPADDING=1>";
            print "<TR><TD BGCOLOR=\"#000000\">\n";
            print "<TABLE CELLSPACING=1 BORDER=0 CELLPADDING=3 width=\"100%\">\n";
        }
    }

    function loggerEnd () {
        if ($this->printlogger) {
            print "</table></td></tr></table>";
        }
    }
    function help ($Full = False) {
        global $PHP_SELF;
        $this->loggerEnd();
        print "From the WebInstaller.php introduction: <p>";

        $file = file(__FILE__);
        foreach($file as $line)
        {
            if ($Full != 2 && strstr($line,"require_once")){
                break;
            }
            $help .= $line;
        }

        highlight_string($help);
        print "<p>";
        if ($Full != 2) {
            print "<a href=\"$PHP_SELF?help=2\">See the full source</a><p>\n";
        }

        print "<a href=\"$PHP_SELF\">Back to the packages overview</A>\n";
    }

}

?>
