<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 5                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2004 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 3.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at the following url:           |
// | http://www.php.net/license/3_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors:   Tomas V.V.Cox <cox@idecnet.com>                           |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

/**
Experimental dependencies database handling functions (not yet in production)

<?php
include './DependencyDB.php';
PEAR::setErrorHandling(PEAR_ERROR_DIE);
$dep = PEAR_DependencyDB::singleton('./test.deps', '/home/cox/www/include/pear');
//$dep->rebuildDB();
$a = $dep->checkAction('uninstall', 'net_socket');
print_r($a);
?>

**/

require_once 'PEAR.php';
require_once 'PEAR/Registry.php';
require_once 'PEAR/Dependency.php';

class PEAR_DependencyDB extends PEAR
{
    // {{{ properties

    var $pear_reg = false;
    var $pear_dep = false;
    var $depdb_file = false;
    var $lockfile = false;
    var $lock_fp = false;
    var $depdb_version = '1.0';

    // }}}
    // {{{ & singleton()

    function &singleton($depdb_file, $reg_file)
    {
        $obj = new PEAR_DependencyDB;
        $reg = &new PEAR_Registry($reg_file);
        $obj->pear_reg = $reg;
        $obj->lockfile = $reg->lockfile;
        $obj->pear_dep = new PEAR_Dependency($reg);
        $obj->depdb_file = $depdb_file;
        $obj->assertDepsDB();
        return $obj;
    }

    // }}}
    // {{{ assertDepsDB()

    function assertDepsDB()
    {
        if (!is_file($this->depdb_file)) {
            $this->rebuildDB();
        } else {
            $depdb = $this->_getDepDB();
            // Datatype format has been changed, rebuild the Deps DB
            if ($depdb['depdb_version'] != $this->depdb_version) {
                $this->rebuildDB();
            }
        }
    }

    // }}}
    // {{{ _lock()

    function _lock($mode = LOCK_EX)
    {
        if (!eregi('Windows 9', php_uname())) {
            if ($mode != LOCK_UN && is_resource($this->lock_fp)) {
                // XXX does not check type of lock (LOCK_SH/LOCK_EX)
                return true;
            }
            $open_mode = 'w';
            // XXX People reported problems with LOCK_SH and 'w'
            if ($mode === LOCK_SH) {
                if (@!is_file($this->lockfile)) {
                    touch($this->lockfile);
                }
                $open_mode = 'r';
            }

            $this->lock_fp = @fopen($this->lockfile, $open_mode);

            if (!is_resource($this->lock_fp)) {
                return $this->raiseError("could not create lock file" .
                                         (isset($php_errormsg) ? ": " . $php_errormsg : ""));
            }
            if (!(int)flock($this->lock_fp, $mode)) {
                switch ($mode) {
                    case LOCK_SH: $str = 'shared';    break;
                    case LOCK_EX: $str = 'exclusive'; break;
                    case LOCK_UN: $str = 'unlock';    break;
                    default:      $str = 'unknown';   break;
                }
                return $this->raiseError("could not acquire $str lock ($this->lockfile)");
            }
        }
        return true;
    }

    // }}}
    // {{{ _unlock()

    function _unlock()
    {
        $ret = $this->_lock(LOCK_UN);
        $this->lock_fp = null;
        return $ret;
    }

    // }}}
    // {{{ rebuildDepsFile()

    function rebuildDB()
    {
        $depdb = array('depdb_version' => $this->depdb_version);
        $packages = $this->pear_reg->listPackages();
        foreach ($packages as $package) {
            $deps = $this->pear_reg->packageInfo($package, 'release_deps');
            $this->setPackageDep($depdb, $package, $deps);
        }
        print_r($depdb);
        $error = $this->_writeDepDB($depdb);
        if (PEAR::isError($error)) {
            return $error;
        }
        return true;
    }

    // }}}
    // {{{ & _getDepDB()

    function &_getDepDB()
    {
        if (!$fp = fopen($this->depdb_file, 'r')) {
            $err = $this->raiseError("Could not open dependencies file `".$this->depdb_file."'");
            return $err;
        }
        $data = unserialize(fread($fp, filesize($this->depdb_file)));
        fclose($fp);
        return $data;
    }

    // }}}
    // {{{ _writeDepDB()

    function _writeDepDB(&$deps)
    {
        if (PEAR::isError($e = $this->_lock(LOCK_EX))) {
            return $e;
        }
        if (!$fp = fopen($this->depdb_file, 'wb')) {
            $this->_unlock();
            return $this->raiseError("Could not open dependencies file `".$this->depfile."' for writting");
        }
        fwrite($fp, serialize($deps));
        fclose($fp);
        $this->_unlock();
        return true;
    }

    // }}}
    /*
    // {{{ removePackageDep()
    function removePackageDep($package)
    {
        $data = &$this->_depGetDepDB();
        if (PEAR::isError($data)) {
            return $data;
        }
        // Other packages depends on this package, can't be removed
        if (isset($data['deps'][$package])) {
            return $data['deps'][$package];
        }
        // The package depends on others, remove those dependencies
        if (isset($data['pkgs'][$package])) {
            foreach ($data['pkgs'][$package] as $pkg => $key) {
                // remove the dependency
                unset($data['deps'][$pkg][$key]);
                // if no more dependencies, remove the subject too
                if (!count($data['deps'][$pkg])) {
                    unset($data['deps'][$pkg]);
                }
            }
            // remove the package from the index list
            unset($data['pkgs'][$package]);
        }
        return $this->_depWriteDepDB();
    }
    // }}}
    */

    // {{{ checkAction()

    function checkAction($action, $package, $new_version = null)
    {
        $depdb = &$this->_getDepDB();
        if (PEAR::isError($depdb)) {
            return $depdb;
        }
        $fails = '';
        switch($action) {
            case 'uninstall':
                // Other packages depends on this package, can't be removed
                if (isset($depdb['deps'][$package])) {
                    foreach ($depdb['deps'][$package] as $dep) {
                        if (!$dep['optional']) {
                            $fails .= "Package '" . $dep['depend'] . "' depends on '$package'\n";
                        }
                    }
                    return $fails;
                }
                return true;
            case 'install':
            case 'upgrade':
                // Other packages depend on this package, check deps. Ex:
                // <dep type='pkg' rel='lt' version='1.0'>Foo</dep> and we are trying to
                // update Foo to version 2.0
                if (isset($depdb['deps'][$package])) {
                    foreach ($depdb['deps'][$package] as $dep) {
                        $relation = $dep['rel'];
                        if ($relation == 'not') {
                            $fails .= "Package '" . $dep['depend'] . "' conflicts with '$package'\n";
                        } elseif ($relation != 'has' && $new_version !== null) {
                            if (!version_compare("$new_version", "{$dep['version']}", $relation) &&
                                !$dep['optional']) {
                                $fails .= "Package '" . $dep['depend'] . "' requires ".
                                          "$package " . $this->pear_dep->signOperator($relation) .
                                          " " . $dep['version'];
                            }
                        }
                    }
                    if (isset($fails)) {
                        return $fails;
                    }
                }
                return true;
        }
    }

    // }}}
    // {{{ commitAction()

    function commitAction($action, $package)
    {

    }

    // }}}
    // {{{ setPackageDep()

    /**
    * Update or insert a the dependencies of a package, prechecking
    * that the package won't break any dependency in the process

    The data structure is as follows:
    $dep_db = array(
        // Other packages depends on this packages
        'deps' => array(
            'Package Name' => array(
                0 => array(
                    // This package depends on 'Package Name'
                    'depend' => 'Package',
                    // Which version 'Package' needs of 'Package Name'
                    'version' => '1.0',
                    // The requirement (version_compare() operator)
                    'rel' => 'ge',
                    // whether the dependency is optional
                    'optional' => true/false
                ),
            ),
        )
        // This packages are dependant on other packages
        'pkgs' => array(
            'Package Dependant' => array(
                // This is a index list with paths over the 'deps' array for quick
                // searching things like "what dependecies has this package?"
                // $dep_db['deps']['Package Name'][3]
                'Package Name' => 3 // key in array ['deps']['Package Name']
            ),
        )
    )

    Note: It only supports package dependencies no other type
    */

    function setPackageDep(&$data, $package, $rel_deps = array())
    {
        // This package has no dependencies
        if (!is_array($rel_deps) || !count($rel_deps)) {
            return true;
        }

        // The package depends on others, register that
        foreach ($rel_deps as $dep) {
            // We only support deps of type 'pkg's
            if ($dep && $dep['type'] == 'pkg' && isset($dep['name'])) {
                $dep_name = strtolower($dep['name']);
                $write = array('depend'  => $package,
                               'rel'     => $dep['rel']);
                if ($dep['rel'] != 'has') {
                    $write['version'] = $dep['version'];
                }
                if (isset($dep['optional']) && $dep['optional'] == 'yes') {
                    $write['optional'] = true;
                } else {
                    $write['optional'] = false;
                }
                settype($data['deps'][$dep_name], 'array');

                // The dependency already exists, update it
                if (isset($data['pkgs'][$package][$dep_name])) {
                    $key = $data['pkgs'][$package][$dep_name];
                    $data['deps'][$dep_name][$key] = $write;

                // New dependency, insert it
                } else {
                    $key = count($data['deps'][$dep_name]);
                    $data['deps'][$dep_name][$key] = $write;
                    settype($data['pkgs'][$package], 'array');
                    $data['pkgs'][$package][$dep_name] = $key;
                }
            }
        }
        return true;
    }
    // }}}
}
?>
