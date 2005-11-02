<?php
/**
 * package.xml generation class, package.xml version 1.0
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
 * needed for PEAR_VALIDATE_* constants
 */
require_once 'PEAR/Validate.php';
require_once 'System.php';
require_once 'PEAR/PackageFile/v2.php';
/**
 * This class converts a PEAR_PackageFile_v1 object into any output format.
 *
 * Supported output formats include array, XML string, and a PEAR_PackageFile_v2
 * object, for converting package.xml 1.0 into package.xml 2.0 with no sweat.
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @PEAR-VER@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_PackageFile_Generator_v1
{
    /**
     * @var PEAR_PackageFile_v1
     */
    var $_packagefile;
    function PEAR_PackageFile_Generator_v1(&$packagefile)
    {
        $this->_packagefile = &$packagefile;
    }

    function getPackagerVersion()
    {
        return '@PEAR-VER@';
    }

    /**
     * @param PEAR_Packager
     * @param bool if true, a .tgz is written, otherwise a .tar is written
     * @param string|null directory in which to save the .tgz
     * @return string|PEAR_Error location of package or error object
     */
    function toTgz(&$packager, $compress = true, $where = null)
    {
        require_once 'Archive/Tar.php';
        if ($where === null) {
            if (!($where = System::mktemp(array('-d')))) {
                return PEAR::raiseError('PEAR_Packagefile_v1::toTgz: mktemp failed');
            }
        } elseif (!@System::mkDir(array('-p', $where))) {
            return PEAR::raiseError('PEAR_Packagefile_v1::toTgz: "' . $where . '" could' .
                ' not be created');
        }
        if (file_exists($where . DIRECTORY_SEPARATOR . 'package.xml') &&
              !is_file($where . DIRECTORY_SEPARATOR . 'package.xml')) {
            return PEAR::raiseError('PEAR_Packagefile_v1::toTgz: unable to save package.xml as' .
                ' "' . $where . DIRECTORY_SEPARATOR . 'package.xml"');
        }
        if (!$this->_packagefile->validate(PEAR_VALIDATE_PACKAGING)) {
            return PEAR::raiseError('PEAR_Packagefile_v1::toTgz: invalid package file');
        }
        $pkginfo = $this->_packagefile->getArray();
        $ext = $compress ? '.tgz' : '.tar';
        $pkgver = $pkginfo['package'] . '-' . $pkginfo['version'];
        $dest_package = getcwd() . DIRECTORY_SEPARATOR . $pkgver . $ext;
        if (file_exists(getcwd() . DIRECTORY_SEPARATOR . $pkgver . $ext) &&
              !is_file(getcwd() . DIRECTORY_SEPARATOR . $pkgver . $ext)) {
            return PEAR::raiseError('PEAR_Packagefile_v1::toTgz: cannot create tgz file "' .
                getcwd() . DIRECTORY_SEPARATOR . $pkgver . $ext . '"');
        }
        if ($pkgfile = $this->_packagefile->getPackageFile()) {
            $pkgdir = dirname(realpath($pkgfile));
            $pkgfile = basename($pkgfile);
        } else {
            return PEAR::raiseError('PEAR_Packagefile_v1::toTgz: package file object must ' .
                'be created from a real file');
        }
        // {{{ Create the package file list
        $filelist = array();
        $i = 0;

        foreach ($this->_packagefile->getFilelist() as $fname => $atts) {
            $file = $pkgdir . DIRECTORY_SEPARATOR . $fname;
            if (!file_exists($file)) {
                return PEAR::raiseError("File does not exist: $fname");
            } else {
                $filelist[$i++] = $file;
                if (!isset($atts['md5sum'])) {
                    $this->_packagefile->setFileAttribute($fname, 'md5sum', md5_file($file));
                }
                $packager->log(2, "Adding file $fname");
            }
        }
        // }}}
        $packagexml = $this->toPackageFile($where, PEAR_VALIDATE_PACKAGING, 'package.xml', true);
        if ($packagexml) {
            $tar =& new Archive_Tar($dest_package, $compress);
            $tar->setErrorHandling(PEAR_ERROR_RETURN); // XXX Don't print errors
            // ----- Creates with the package.xml file
            $ok = $tar->createModify(array($packagexml), '', $where);
            if (PEAR::isError($ok)) {
                return $ok;
            } elseif (!$ok) {
                return PEAR::raiseError('PEAR_Packagefile_v1::toTgz: tarball creation failed');
            }
            // ----- Add the content of the package
            if (!$tar->addModify($filelist, $pkgver, $pkgdir)) {
                return PEAR::raiseError('PEAR_Packagefile_v1::toTgz: tarball creation failed');
            }
            return $dest_package;
        }
    }

    /**
     * @param string|null directory to place the package.xml in, or null for a temporary dir
     * @param int one of the PEAR_VALIDATE_* constants
     * @param string name of the generated file
     * @param bool if true, then no analysis will be performed on role="php" files
     * @return string|PEAR_Error path to the created file on success
     */
    function toPackageFile($where = null, $state = PEAR_VALIDATE_NORMAL, $name = 'package.xml',
                           $nofilechecking = false)
    {
        if (!$this->_packagefile->validate($state, $nofilechecking)) {
            return PEAR::raiseError('PEAR_Packagefile_v1::toPackageFile: invalid package.xml',
                null, null, null, $this->_packagefile->getValidationWarnings());
        }
        if ($where === null) {
            if (!($where = System::mktemp(array('-d')))) {
                return PEAR::raiseError('PEAR_Packagefile_v1::toPackageFile: mktemp failed');
            }
        } elseif (!@System::mkDir(array('-p', $where))) {
            return PEAR::raiseError('PEAR_Packagefile_v1::toPackageFile: "' . $where . '" could' .
                ' not be created');
        }
        $newpkgfile = $where . DIRECTORY_SEPARATOR . $name;
        $np = @fopen($newpkgfile, 'wb');
        if (!$np) {
            return PEAR::raiseError('PEAR_Packagefile_v1::toPackageFile: unable to save ' .
               "$name as $newpkgfile");
        }
        fwrite($np, $this->toXml($state, true));
        fclose($np);
        return $newpkgfile;
    }

    /**
     * fix both XML encoding to be UTF8, and replace standard XML entities < > " & '
     *
     * @param string $string
     * @return string
     * @access private
     */
    function _fixXmlEncoding($string)
    {
        if (version_compare(phpversion(), '5.0.0', 'lt')) {
            $string = utf8_encode($string);
        }
        return strtr($string, array(
                                          '&'  => '&amp;',
                                          '>'  => '&gt;',
                                          '<'  => '&lt;',
                                          '"'  => '&quot;',
                                          '\'' => '&apos;' ));
    }

    /**
     * Return an XML document based on the package info (as returned
     * by the PEAR_Common::infoFrom* methods).
     *
     * @return string XML data
     */
    function toXml($state = PEAR_VALIDATE_NORMAL, $nofilevalidation = false)
    {
        $this->_packagefile->setDate(date('Y-m-d'));
        if (!$this->_packagefile->validate($state, $nofilevalidation)) {
            return false;
        }
        $pkginfo = $this->_packagefile->getArray();
        static $maint_map = array(
            "handle" => "user",
            "name" => "name",
            "email" => "email",
            "role" => "role",
            );
        $ret = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
        $ret .= "<!DOCTYPE package SYSTEM \"http://pear.php.net/dtd/package-1.0\">\n";
        $ret .= "<package version=\"1.0\" packagerversion=\"@PEAR-VER@\">\n" .
" <name>$pkginfo[package]</name>";
        if (isset($pkginfo['extends'])) {
            $ret .= "\n<extends>$pkginfo[extends]</extends>";
        }
        $ret .=
 "\n <summary>".$this->_fixXmlEncoding($pkginfo['summary'])."</summary>\n" .
" <description>".trim($this->_fixXmlEncoding($pkginfo['description']))."\n </description>\n" .
" <maintainers>\n";
        foreach ($pkginfo['maintainers'] as $maint) {
            $ret .= "  <maintainer>\n";
            foreach ($maint_map as $idx => $elm) {
                $ret .= "   <$elm>";
                $ret .= $this->_fixXmlEncoding($maint[$idx]);
                $ret .= "</$elm>\n";
            }
            $ret .= "  </maintainer>\n";
        }
        $ret .= "  </maintainers>\n";
        $ret .= $this->_makeReleaseXml($pkginfo, false, $state);
        if (isset($pkginfo['changelog']) && count($pkginfo['changelog']) > 0) {
            $ret .= " <changelog>\n";
            foreach ($pkginfo['changelog'] as $oldrelease) {
                $ret .= $this->_makeReleaseXml($oldrelease, true);
            }
            $ret .= " </changelog>\n";
        }
        $ret .= "</package>\n";
        return $ret;
    }

    // }}}
    // {{{ _makeReleaseXml()

    /**
     * Generate part of an XML description with release information.
     *
     * @param array  $pkginfo    array with release information
     * @param bool   $changelog  whether the result will be in a changelog element
     *
     * @return string XML data
     *
     * @access private
     */
    function _makeReleaseXml($pkginfo, $changelog = false, $state = PEAR_VALIDATE_NORMAL)
    {
        // XXX QUOTE ENTITIES IN PCDATA, OR EMBED IN CDATA BLOCKS!!
        $indent = $changelog ? "  " : "";
        $ret = "$indent <release>\n";
        if (!empty($pkginfo['version'])) {
            $ret .= "$indent  <version>$pkginfo[version]</version>\n";
        }
        if (!empty($pkginfo['release_date'])) {
            $ret .= "$indent  <date>$pkginfo[release_date]</date>\n";
        }
        if (!empty($pkginfo['release_license'])) {
            $ret .= "$indent  <license>$pkginfo[release_license]</license>\n";
        }
        if (!empty($pkginfo['release_state'])) {
            $ret .= "$indent  <state>$pkginfo[release_state]</state>\n";
        }
        if (!empty($pkginfo['release_notes'])) {
            $ret .= "$indent  <notes>".trim($this->_fixXmlEncoding($pkginfo['release_notes']))
            ."\n$indent  </notes>\n";
        }
        if (!empty($pkginfo['release_warnings'])) {
            $ret .= "$indent  <warnings>".$this->_fixXmlEncoding($pkginfo['release_warnings'])."</warnings>\n";
        }
        if (isset($pkginfo['release_deps']) && sizeof($pkginfo['release_deps']) > 0) {
            $ret .= "$indent  <deps>\n";
            foreach ($pkginfo['release_deps'] as $dep) {
                $ret .= "$indent   <dep type=\"$dep[type]\" rel=\"$dep[rel]\"";
                if (isset($dep['version'])) {
                    $ret .= " version=\"$dep[version]\"";
                }
                if (isset($dep['optional'])) {
                    $ret .= " optional=\"$dep[optional]\"";
                }
                if (isset($dep['name'])) {
                    $ret .= ">$dep[name]</dep>\n";
                } else {
                    $ret .= "/>\n";
                }
            }
            $ret .= "$indent  </deps>\n";
        }
        if (isset($pkginfo['configure_options'])) {
            $ret .= "$indent  <configureoptions>\n";
            foreach ($pkginfo['configure_options'] as $c) {
                $ret .= "$indent   <configureoption name=\"".
                    $this->_fixXmlEncoding($c['name']) . "\"";
                if (isset($c['default'])) {
                    $ret .= " default=\"" . $this->_fixXmlEncoding($c['default']) . "\"";
                }
                $ret .= " prompt=\"" . $this->_fixXmlEncoding($c['prompt']) . "\"";
                $ret .= "/>\n";
            }
            $ret .= "$indent  </configureoptions>\n";
        }
        if (isset($pkginfo['provides'])) {
            foreach ($pkginfo['provides'] as $key => $what) {
                $ret .= "$indent  <provides type=\"$what[type]\" ";
                $ret .= "name=\"$what[name]\" ";
                if (isset($what['extends'])) {
                    $ret .= "extends=\"$what[extends]\" ";
                }
                $ret .= "/>\n";
            }
        }
        if (isset($pkginfo['filelist'])) {
            $ret .= "$indent  <filelist>\n";
            if ($state ^ PEAR_VALIDATE_PACKAGING) {
                $ret .= $this->recursiveXmlFilelist($pkginfo['filelist']);
            } else {
                foreach ($pkginfo['filelist'] as $file => $fa) {
                    @$ret .= "$indent   <file role=\"$fa[role]\"";
                    if (isset($fa['baseinstalldir'])) {
                        $ret .= ' baseinstalldir="' .
                            $this->_fixXmlEncoding($fa['baseinstalldir']) . '"';
                    }
                    if (isset($fa['md5sum'])) {
                        $ret .= " md5sum=\"$fa[md5sum]\"";
                    }
                    if (isset($fa['platform'])) {
                        $ret .= " platform=\"$fa[platform]\"";
                    }
                    if (!empty($fa['install-as'])) {
                        $ret .= ' install-as="' .
                            $this->_fixXmlEncoding($fa['install-as']) . '"';
                    }
                    $ret .= ' name="' . $this->_fixXmlEncoding($file) . '"';
                    if (empty($fa['replacements'])) {
                        $ret .= "/>\n";
                    } else {
                        $ret .= ">\n";
                        foreach ($fa['replacements'] as $r) {
                            $ret .= "$indent    <replace";
                            foreach ($r as $k => $v) {
                                $ret .= " $k=\"" . $this->_fixXmlEncoding($v) .'"';
                            }
                            $ret .= "/>\n";
                        }
                        @$ret .= "$indent   </file>\n";
                    }
                }
            }
            $ret .= "$indent  </filelist>\n";
        }
        $ret .= "$indent </release>\n";
        return $ret;
    }

    /**
     * @param array
     * @access protected
     */
    function recursiveXmlFilelist($list)
    {
        $this->_dirs = array();
        foreach ($list as $file => $attributes) {
            $this->_addDir($this->_dirs, explode('/', dirname($file)), $file, $attributes);
        }
        return $this->_formatDir($this->_dirs);
    }

    /**
     * @param array
     * @param array
     * @param string|null
     * @param array|null
     * @access private
     */
    function _addDir(&$dirs, $dir, $file = null, $attributes = null)
    {
        if ($dir == array() || $dir == array('.')) {
            $dirs['files'][basename($file)] = $attributes;
            return;
        }
        $curdir = array_shift($dir);
        if (!isset($dirs['dirs'][$curdir])) {
            $dirs['dirs'][$curdir] = array();
        }
        $this->_addDir($dirs['dirs'][$curdir], $dir, $file, $attributes);
    }

    /**
     * @param array
     * @param string
     * @param string
     * @access private
     */
    function _formatDir($dirs, $indent = '', $curdir = '')
    {
        $ret = '';
        if (!count($dirs)) {
            return '';
        }
        if (isset($dirs['dirs'])) {
            uksort($dirs['dirs'], 'strnatcasecmp');
            foreach ($dirs['dirs'] as $dir => $contents) {
                $usedir = "$curdir/$dir";
                $ret .= "$indent   <dir name=\"$dir\">\n";
                $ret .= $this->_formatDir($contents, "$indent ", $usedir);
                $ret .= "$indent   </dir> <!-- $usedir -->\n";
            }
        }
        if (isset($dirs['files'])) {
            uksort($dirs['files'], 'strnatcasecmp');
            foreach ($dirs['files'] as $file => $attribs) {
                $ret .= $this->_formatFile($file, $attribs, $indent);
            }
        }
        return $ret;
    }

    /**
     * @param string
     * @param array
     * @param string
     * @access private
     */
    function _formatFile($file, $attributes, $indent)
    {
        $ret = "$indent   <file role=\"$attributes[role]\"";
        if (isset($attributes['baseinstalldir'])) {
            $ret .= ' baseinstalldir="' .
                $this->_fixXmlEncoding($attributes['baseinstalldir']) . '"';
        }
        if (isset($attributes['md5sum'])) {
            $ret .= " md5sum=\"$attributes[md5sum]\"";
        }
        if (isset($attributes['platform'])) {
            $ret .= " platform=\"$attributes[platform]\"";
        }
        if (!empty($attributes['install-as'])) {
            $ret .= ' install-as="' .
                $this->_fixXmlEncoding($attributes['install-as']) . '"';
        }
        $ret .= ' name="' . $this->_fixXmlEncoding($file) . '"';
        if (empty($attributes['replacements'])) {
            $ret .= "/>\n";
        } else {
            $ret .= ">\n";
            foreach ($attributes['replacements'] as $r) {
                $ret .= "$indent    <replace";
                foreach ($r as $k => $v) {
                    $ret .= " $k=\"" . $this->_fixXmlEncoding($v) .'"';
                }
                $ret .= "/>\n";
            }
            $ret .= "$indent   </file>\n";
        }
        return $ret;
    }

    // {{{ _unIndent()

    /**
     * Unindent given string (?)
     *
     * @param string $str The string that has to be unindented.
     * @return string
     * @access private
     */
    function _unIndent($str)
    {
        // remove leading newlines
        $str = preg_replace('/^[\r\n]+/', '', $str);
        // find whitespace at the beginning of the first line
        $indent_len = strspn($str, " \t");
        $indent = substr($str, 0, $indent_len);
        $data = '';
        // remove the same amount of whitespace from following lines
        foreach (explode("\n", $str) as $line) {
            if (substr($line, 0, $indent_len) == $indent) {
                $data .= substr($line, $indent_len) . "\n";
            }
        }
        return $data;
    }

    /**
     * @return array
     */
    function dependenciesToV2()
    {
        $arr = array();
        $this->_convertDependencies2_0($arr);
        return $arr['dependencies'];
    }

    /**
     * Convert a package.xml version 1.0 into version 2.0
     *
     * Note that this does a basic conversion, to allow more advanced
     * features like bundles and multiple releases
     * @param string the classname to instantiate and return.  This must be
     *               PEAR_PackageFile_v2 or a descendant
     * @param boolean if true, only valid, deterministic package.xml 1.0 as defined by the
     *                strictest parameters will be converted
     * @return PEAR_PackageFile_v2|PEAR_Error
     */
    function &toV2($class = 'PEAR_PackageFile_v2', $strict = false)
    {
        if ($strict) {
            if (!$this->_packagefile->validate()) {
                $a = PEAR::raiseError('invalid package.xml version 1.0 cannot be converted' .
                    ' to version 2.0', null, null, null,
                    $this->_packagefile->getValidationWarnings(true));
                return $a;
            }
        }
        $arr = array(
            'attribs' => array(
                             'version' => '2.0',
                             'xmlns' => 'http://pear.php.net/dtd/package-2.0',
                             'xmlns:tasks' => 'http://pear.php.net/dtd/tasks-1.0',
                             'xmlns:xsi' => 'http://www.w3.org/2001/XMLSchema-instance',
                             'xsi:schemaLocation' => "http://pear.php.net/dtd/tasks-1.0\n" .
"http://pear.php.net/dtd/tasks-1.0.xsd\n" .
"http://pear.php.net/dtd/package-2.0\n" .
'http://pear.php.net/dtd/package-2.0.xsd',
                         ),
            'name' => $this->_packagefile->getPackage(),
            'channel' => 'pear.php.net',
        );
        $arr['summary'] = $this->_packagefile->getSummary();
        $arr['description'] = $this->_packagefile->getDescription();
        $maintainers = $this->_packagefile->getMaintainers();
        foreach ($maintainers as $maintainer) {
            if ($maintainer['role'] != 'lead') {
                continue;
            }
            $new = array(
                'name' => $maintainer['name'],
                'user' => $maintainer['handle'],
                'email' => $maintainer['email'],
                'active' => 'yes',
            );
            $arr['lead'][] = $new;
        }
        if (!isset($arr['lead'])) { // some people... you know?
            $arr['lead'] = array(
                'name' => 'unknown',
                'user' => 'unknown',
                'email' => 'noleadmaintainer@example.com',
                'active' => 'no',
            );
        }
        if (count($arr['lead']) == 1) {
            $arr['lead'] = $arr['lead'][0];
        }
        foreach ($maintainers as $maintainer) {
            if ($maintainer['role'] == 'lead') {
                continue;
            }
            $new = array(
                'name' => $maintainer['name'],
                'user' => $maintainer['handle'],
                'email' => $maintainer['email'],
                'active' => 'yes',
            );
            $arr[$maintainer['role']][] = $new;
        }
        if (isset($arr['developer']) && count($arr['developer']) == 1) {
            $arr['developer'] = $arr['developer'][0];
        }
        if (isset($arr['contributor']) && count($arr['contributor']) == 1) {
            $arr['contributor'] = $arr['contributor'][0];
        }
        if (isset($arr['helper']) && count($arr['helper']) == 1) {
            $arr['helper'] = $arr['helper'][0];
        }
        $arr['date'] = $this->_packagefile->getDate();
        $arr['version'] =
            array(
                'release' => $this->_packagefile->getVersion(),
                'api' => $this->_packagefile->getVersion(),
            );
        $arr['stability'] =
            array(
                'release' => $this->_packagefile->getState(),
                'api' => $this->_packagefile->getState(),
            );
        $licensemap =
            array(
                'php' => 'http://www.php.net/license',
                'php license' => 'http://www.php.net/license',
                'lgpl' => 'http://www.gnu.org/copyleft/lesser.html',
                'bsd' => 'http://www.opensource.org/licenses/bsd-license.php',
                'bsd style' => 'http://www.opensource.org/licenses/bsd-license.php',
                'bsd-style' => 'http://www.opensource.org/licenses/bsd-license.php',
                'mit' => 'http://www.opensource.org/licenses/mit-license.php',
                'gpl' => 'http://www.gnu.org/copyleft/gpl.html',
                'apache' => 'http://www.opensource.org/licenses/apache2.0.php'
            );
        if (isset($licensemap[strtolower($this->_packagefile->getLicense())])) {
            $arr['license'] = array(
                'attribs' => array('uri' =>
                    $licensemap[strtolower($this->_packagefile->getLicense())]),
                '_content' => $this->_packagefile->getLicense()
                );
        } else {
            // don't use bogus uri
            $arr['license'] = $this->_packagefile->getLicense();
        }
        $arr['notes'] = $this->_packagefile->getNotes();
        $temp = array();
        $arr['contents'] = $this->_convertFilelist2_0($temp);
        $this->_convertDependencies2_0($arr);
        $release = ($this->_packagefile->getConfigureOptions() || $this->_isExtension) ?
            'extsrcrelease' : 'phprelease';
        if ($release == 'extsrcrelease') {
            $arr['channel'] = 'pecl.php.net';
            $arr['providesextension'] = $arr['name']; // assumption
        }
        $arr[$release] = array();
        if ($this->_packagefile->getConfigureOptions()) {
            $arr[$release]['configureoption'] = $this->_packagefile->getConfigureOptions();
            foreach ($arr[$release]['configureoption'] as $i => $opt) {
                $arr[$release]['configureoption'][$i] = array('attribs' => $opt);
            }
            if (count($arr[$release]['configureoption']) == 1) {
                $arr[$release]['configureoption'] = $arr[$release]['configureoption'][0];
            }
        }
        $this->_convertRelease2_0($arr[$release], $temp);
        if ($release == 'extsrcrelease' && count($arr[$release]) > 1) {
            // multiple extsrcrelease tags added in PEAR 1.4.1
            $arr['dependencies']['required']['pearinstaller']['min'] = '1.4.1';
        }
        if ($cl = $this->_packagefile->getChangelog()) {
            foreach ($cl as $release) {
                $rel = array();
                $rel['version'] =
                    array(
                        'release' => $release['version'],
                        'api' => $release['version'],
                    );
                if (!isset($release['release_state'])) {
                    $release['release_state'] = 'stable';
                }
                $rel['stability'] =
                    array(
                        'release' => $release['release_state'],
                        'api' => $release['release_state'],
                    );
                if (isset($release['release_date'])) {
                    $rel['date'] = $release['release_date'];
                } else {
                    $rel['date'] = date('Y-m-d');
                }
                if (isset($release['release_license'])) {
                    if (isset($licensemap[strtolower($release['release_license'])])) {
                        $uri = $licensemap[strtolower($release['release_license'])];
                    } else {
                        $uri = 'http://www.example.com';
                    }
                    $rel['license'] = array(
                            'attribs' => array('uri' => $uri),
                            '_content' => $release['release_license']
                        );
                } else {
                    $rel['license'] = $arr['license'];
                }
                if (!isset($release['release_notes'])) {
                    $release['release_notes'] = 'no release notes';
                }
                $rel['notes'] = $release['release_notes'];
                $arr['changelog']['release'][] = $rel;
            }
        }
        $ret = new $class;
        $ret->setConfig($this->_packagefile->_config);
        $ret->setLogger($this->_packagefile->_logger);
        $ret->fromArray($arr);
        return $ret;
    }

    /**
     * @param array
     * @param bool
     * @access private
     */
    function _convertDependencies2_0(&$release, $internal = false)
    {
        $peardep = array('pearinstaller' =>
            array('min' => '1.4.0b1')); // this is a lot safer
        $required = $optional = array();
        $release['dependencies'] = array();
        if ($this->_packagefile->hasDeps()) {
            foreach ($this->_packagefile->getDeps() as $dep) {
                if (!isset($dep['optional']) || $dep['optional'] == 'no') {
                    $required[] = $dep;
                } else {
                    $optional[] = $dep;
                }
            }
            foreach (array('required', 'optional') as $arr) {
                $deps = array();
                foreach ($$arr as $dep) {
                    // organize deps by dependency type and name
                    if (!isset($deps[$dep['type']])) {
                        $deps[$dep['type']] = array();
                    }
                    if (isset($dep['name'])) {
                        $deps[$dep['type']][$dep['name']][] = $dep;
                    } else {
                        $deps[$dep['type']][] = $dep;
                    }
                }
                do {
                    if (isset($deps['php'])) {
                        $php = array();
                        if (count($deps['php']) > 1) {
                            $php = $this->_processPhpDeps($deps['php']);
                        } else {
                            if (!isset($deps['php'][0])) {
                                list($key, $blah) = each ($deps['php']); // stupid buggy versions
                                $deps['php'] = array($blah[0]);
                            }
                            $php = $this->_processDep($deps['php'][0]);
                            if (!$php) {
                                break; // poor mans throw
                            }
                        }
                        $release['dependencies'][$arr]['php'] = $php;
                    }
                } while (false);
                do {
                    if (isset($deps['pkg'])) {
                        $pkg = array();
                        $pkg = $this->_processMultipleDepsName($deps['pkg']);
                        if (!$pkg) {
                            break; // poor mans throw
                        }
                        $release['dependencies'][$arr]['package'] = $pkg;
                    }
                } while (false);
                do {
                    if (isset($deps['ext'])) {
                        $pkg = array();
                        $pkg = $this->_processMultipleDepsName($deps['ext']);
                        $release['dependencies'][$arr]['extension'] = $pkg;
                    }
                } while (false);
                // skip sapi - it's not supported so nobody will have used it
                // skip os - it's not supported in 1.0
            }
        }
        if (isset($release['dependencies']['required'])) {
            $release['dependencies']['required'] =
                array_merge($peardep, $release['dependencies']['required']);
        } else {
            $release['dependencies']['required'] = $peardep;
        }
        if (!isset($release['dependencies']['required']['php'])) {
            $release['dependencies']['required']['php'] =
                array('min' => '4.0.0');
        }
        $order = array();
        $bewm = $release['dependencies']['required'];
        $order['php'] = $bewm['php'];
        $order['pearinstaller'] = $bewm['pearinstaller'];
        isset($bewm['package']) ? $order['package'] = $bewm['package'] :0;
        isset($bewm['extension']) ? $order['extension'] = $bewm['extension'] :0;
        $release['dependencies']['required'] = $order;
    }

    /**
     * @param array
     * @access private
     */
    function _convertFilelist2_0(&$package)
    {
        $ret = array('dir' =>
                    array(
                        'attribs' => array('name' => '/'),
                        'file' => array()
                        )
                    );
        $package['platform'] =
        $package['install-as'] = array();
        $this->_isExtension = false;
        foreach ($this->_packagefile->getFilelist() as $name => $file) {
            $file['name'] = $name;
            if (isset($file['role']) && $file['role'] == 'src') {
                $this->_isExtension = true;
            }
            if (isset($file['replacements'])) {
                $repl = $file['replacements'];
                unset($file['replacements']);
            } else {
                unset($repl);
            }
            if (isset($file['install-as'])) {
                $package['install-as'][$name] = $file['install-as'];
                unset($file['install-as']);
            }
            if (isset($file['platform'])) {
                $package['platform'][$name] = $file['platform'];
                unset($file['platform']);
            }
            $file = array('attribs' => $file);
            if (isset($repl)) {
                foreach ($repl as $replace ) {
                    $file['tasks:replace'][] = array('attribs' => $replace);
                }
                if (count($repl) == 1) {
                    $file['tasks:replace'] = $file['tasks:replace'][0];
                }
            }
            $ret['dir']['file'][] = $file;
        }
        return $ret;
    }

    /**
     * Post-process special files with install-as/platform attributes and
     * make the release tag.
     * 
     * This complex method follows this work-flow to create the release tags:
     * 
     * <pre>
     * - if any install-as/platform exist, create a generic release and fill it with
     *   o <install as=..> tags for <file name=... install-as=...>
     *   o <install as=..> tags for <file name=... platform=!... install-as=..>
     *   o <ignore> tags for <file name=... platform=...>
     *   o <ignore> tags for <file name=... platform=... install-as=..>
     * - create a release for each platform encountered and fill with
     *   o <install as..> tags for <file name=... install-as=...>
     *   o <install as..> tags for <file name=... platform=this platform install-as=..>
     *   o <install as..> tags for <file name=... platform=!other platform install-as=..>
     *   o <ignore> tags for <file name=... platform=!this platform>
     *   o <ignore> tags for <file name=... platform=other platform>
     *   o <ignore> tags for <file name=... platform=other platform install-as=..>
     *   o <ignore> tags for <file name=... platform=!this platform install-as=..>
     * </pre>
     * 
     * It does this by accessing the $package parameter, which contains an array with
     * indices:
     * 
     *  - platform: mapping of file => OS the file should be installed on
     *  - install-as: mapping of file => installed name
     *  - osmap: mapping of OS => list of files that should be installed
     *    on that OS
     *  - notosmap: mapping of OS => list of files that should not be
     *    installed on that OS
     *
     * @param array
     * @param array
     * @access private
     */
    function _convertRelease2_0(&$release, $package)
    {
        //- if any install-as/platform exist, create a generic release and fill it with 
        if (count($package['platform']) || count($package['install-as'])) {
            $generic = array();
            $genericIgnore = array();
            foreach ($package['install-as'] as $file => $as) {
                //o <install as=..> tags for <file name=... install-as=...>
                if (!isset($package['platform'][$file])) {
                    $generic[] = $file;
                    continue;
                }
                //o <install as=..> tags for <file name=... platform=!... install-as=..>
                if (isset($package['platform'][$file]) &&
                      $package['platform'][$file]{0} == '!') {
                    $generic[] = $file;
                    continue;
                }
                //o <ignore> tags for <file name=... platform=... install-as=..>
                if (isset($package['platform'][$file]) &&
                      $package['platform'][$file]{0} != '!') {
                    $genericIgnore[] = $file;
                    continue;
                }
            }
            foreach ($package['platform'] as $file => $platform) {
                if (isset($package['install-as'][$file])) {
                    continue;
                }
                if ($platform{0} != '!') {
                    //o <ignore> tags for <file name=... platform=...>
                    $genericIgnore[] = $file;
                }
            }
            if (count($package['platform'])) {
                $oses = $notplatform = $platform = array();
                foreach ($package['platform'] as $file => $os) {
                    // get a list of oses
                    if ($os{0} == '!') {
                        if (isset($oses[substr($os, 1)])) {
                            continue;
                        }
                        $oses[substr($os, 1)] = count($oses);
                    } else {
                        if (isset($oses[$os])) {
                            continue;
                        }
                        $oses[$os] = count($oses);
                    }
                }
                //- create a release for each platform encountered and fill with
                foreach ($oses as $os => $releaseNum) {
                    $release[$releaseNum]['installconditions']['os']['name'] = $os;
                    $release[$releaseNum]['filelist'] = array('install' => array(),
                        'ignore' => array());
                    foreach ($package['install-as'] as $file => $as) {
                        //o <install as=..> tags for <file name=... install-as=...>
                        if (!isset($package['platform'][$file])) {
                            $release[$releaseNum]['filelist']['install'][] =
                                array(
                                    'attribs' => array(
                                        'name' => $file,
                                        'as' => $as,
                                    ),
                                );
                            continue;
                        }
                        //o <install as..> tags for
                        //  <file name=... platform=this platform install-as=..>
                        if (isset($package['platform'][$file]) &&
                              $package['platform'][$file] == $os) {
                            $release[$releaseNum]['filelist']['install'][] =
                                array(
                                    'attribs' => array(
                                        'name' => $file,
                                        'as' => $as,
                                    ),
                                );
                            continue;
                        }
                        //o <install as..> tags for
                        //  <file name=... platform=!other platform install-as=..>
                        if (isset($package['platform'][$file]) &&
                              $package['platform'][$file] != "!$os" &&
                              $package['platform'][$file]{0} == '!') {
                            $release[$releaseNum]['filelist']['install'][] =
                                array(
                                    'attribs' => array(
                                        'name' => $file,
                                        'as' => $as,
                                    ),
                                );
                            continue;
                        }
                        //o <ignore> tags for
                        //  <file name=... platform=!this platform install-as=..>
                        if (isset($package['platform'][$file]) &&
                              $package['platform'][$file] == "!$os") {
                            $release[$releaseNum]['filelist']['ignore'][] =
                                array(
                                    'attribs' => array(
                                        'name' => $file,
                                    ),
                                );
                            continue;
                        }
                        //o <ignore> tags for
                        //  <file name=... platform=other platform install-as=..>
                        if (isset($package['platform'][$file]) &&
                              $package['platform'][$file]{0} != '!' &&
                              $package['platform'][$file] != $os) {
                            $release[$releaseNum]['filelist']['ignore'][] =
                                array(
                                    'attribs' => array(
                                        'name' => $file,
                                    ),
                                );
                            continue;
                        }
                    }
                    foreach ($package['platform'] as $file => $platform) {
                        if (isset($package['install-as'][$file])) {
                            continue;
                        }
                        //o <ignore> tags for <file name=... platform=!this platform>
                        if ($platform == "!$os") {
                            $release[$releaseNum]['filelist']['ignore'][] =
                                array(
                                    'attribs' => array(
                                        'name' => $file,
                                    ),
                                );
                            continue;
                        }
                        //o <ignore> tags for <file name=... platform=other platform>
                        if ($platform{0} != '!' && $platform != $os) {
                            $release[$releaseNum]['filelist']['ignore'][] =
                                array(
                                    'attribs' => array(
                                        'name' => $file,
                                    ),
                                );
                        }
                    }
                    if (!count($release[$releaseNum]['filelist']['install'])) {
                        unset($release[$releaseNum]['filelist']['install']);
                    }
                    if (!count($release[$releaseNum]['filelist']['ignore'])) {
                        unset($release[$releaseNum]['filelist']['ignore']);
                    }
                }
                if (count($generic) || count($genericIgnore)) {
                    $release[count($oses)] = array();
                    if (count($generic)) {
                        foreach ($generic as $file) {
                            if (isset($package['install-as'][$file])) {
                                $installas = $package['install-as'][$file];
                            } else {
                                $installas = $file;
                            }
                            $release[count($oses)]['filelist']['install'][] =
                                array(
                                    'attribs' => array(
                                        'name' => $file,
                                        'as' => $installas,
                                    )
                                );
                        }
                    }
                    if (count($genericIgnore)) {
                        foreach ($genericIgnore as $file) {
                            $release[count($oses)]['filelist']['ignore'][] =
                                array(
                                    'attribs' => array(
                                        'name' => $file,
                                    )
                                );
                        }
                    }
                }
                // cleanup
                foreach ($release as $i => $rel) {
                    if (isset($rel['filelist']['install']) &&
                          count($rel['filelist']['install']) == 1) {
                        $release[$i]['filelist']['install'] =
                            $release[$i]['filelist']['install'][0];
                    }
                    if (isset($rel['filelist']['ignore']) &&
                          count($rel['filelist']['ignore']) == 1) {
                        $release[$i]['filelist']['ignore'] =
                            $release[$i]['filelist']['ignore'][0];
                    }
                }
                if (count($release) == 1) {
                    $release = $release[0];
                }
            } else {
                // no platform atts, but some install-as atts
                foreach ($package['install-as'] as $file => $value) {
                    $release['filelist']['install'][] =
                        array(
                            'attribs' => array(
                                'name' => $file,
                                'as' => $value
                            )
                        );
                }
                if (count($release['filelist']['install']) == 1) {
                    $release['filelist']['install'] = $release['filelist']['install'][0];
                }
            }
        }
    }

    /**
     * @param array
     * @return array
     * @access private
     */
    function _processDep($dep)
    {
        if ($dep['type'] == 'php') {
            if ($dep['rel'] == 'has') {
                // come on - everyone has php!
                return false;
            }
        }
        $php = array();
        if ($dep['type'] != 'php') {
            $php['name'] = $dep['name'];
            if ($dep['type'] == 'pkg') {
                $php['channel'] = 'pear.php.net';
            }
        }
        switch ($dep['rel']) {
            case 'gt' :
                $php['min'] = $dep['version'];
                $php['exclude'] = $dep['version'];
            break;
            case 'ge' :
                if (!isset($dep['version'])) {
                    if ($dep['type'] == 'php') {
                        if (isset($dep['name'])) {
                            $dep['version'] = $dep['name'];
                        }
                    }
                }
                $php['min'] = $dep['version'];
            break;
            case 'lt' :
                $php['max'] = $dep['version'];
                $php['exclude'] = $dep['version'];
            break;
            case 'le' :
                $php['max'] = $dep['version'];
            break;
            case 'eq' :
                $php['min'] = $dep['version'];
                $php['max'] = $dep['version'];
            break;
            case 'ne' :
                $php['exclude'] = $dep['version'];
            break;
            case 'not' :
                $php['conflicts'] = 'yes';
            break;
        }
        return $php;
    }

    /**
     * @param array
     * @return array
     */
    function _processPhpDeps($deps)
    {
        $test = array();
        foreach ($deps as $dep) {
            $test[] = $this->_processDep($dep);
        }
        $min = array();
        $max = array();
        foreach ($test as $dep) {
            if (!$dep) {
                continue;
            }
            if (isset($dep['min'])) {
                $min[$dep['min']] = count($min);
            }
            if (isset($dep['max'])) {
                $max[$dep['max']] = count($max);
            }
        }
        if (count($min) > 0) {
            uksort($min, 'version_compare');
        }
        if (count($max) > 0) {
            uksort($max, 'version_compare');
        }
        if (count($min)) {
            // get the highest minimum
            $min = array_pop($a = array_flip($min));
        } else {
            $min = false;
        }
        if (count($max)) {
            // get the lowest maximum
            $max = array_shift($a = array_flip($max));
        } else {
            $max = false;
        }
        if ($min) {
            $php['min'] = $min;
        }
        if ($max) {
            $php['max'] = $max;
        }
        $exclude = array();
        foreach ($test as $dep) {
            if (!isset($dep['exclude'])) {
                continue;
            }
            $exclude[] = $dep['exclude'];
        }
        if (count($exclude)) {
            $php['exclude'] = $exclude;
        }
        return $php;
    }

    /**
     * process multiple dependencies that have a name, like package deps
     * @param array
     * @return array
     * @access private
     */
    function _processMultipleDepsName($deps)
    {
        $tests = array();
        foreach ($deps as $name => $dep) {
            foreach ($dep as $d) {
                $tests[$name][] = $this->_processDep($d);
            }
        }
        foreach ($tests as $name => $test) {
            $php = array();
            $min = array();
            $max = array();
            $php['name'] = $name;
            foreach ($test as $dep) {
                if (!$dep) {
                    continue;
                }
                if (isset($dep['channel'])) {
                    $php['channel'] = 'pear.php.net';
                }
                if (isset($dep['conflicts']) && $dep['conflicts'] == 'yes') {
                    $php['conflicts'] = 'yes';
                }
                if (isset($dep['min'])) {
                    $min[$dep['min']] = count($min);
                }
                if (isset($dep['max'])) {
                    $max[$dep['max']] = count($max);
                }
            }
            if (count($min) > 0) {
                uksort($min, 'version_compare');
            }
            if (count($max) > 0) {
                uksort($max, 'version_compare');
            }
            if (count($min)) {
                // get the highest minimum
                $min = array_pop($a = array_flip($min));
            } else {
                $min = false;
            }
            if (count($max)) {
                // get the lowest maximum
                $max = array_shift($a = array_flip($max));
            } else {
                $max = false;
            }
            if ($min) {
                $php['min'] = $min;
            }
            if ($max) {
                $php['max'] = $max;
            }
            $exclude = array();
            foreach ($test as $dep) {
                if (!isset($dep['exclude'])) {
                    continue;
                }
                $exclude[] = $dep['exclude'];
            }
            if (count($exclude)) {
                $php['exclude'] = $exclude;
            }
            $ret[] = $php;
        }
        return $ret;
    }
}
?>