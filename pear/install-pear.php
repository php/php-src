<?php

/* $Id$ */

$pear_dir = dirname(__FILE__);
ini_set('include_path', $pear_dir);
include_once 'PEAR.php';
include_once 'PEAR/Installer.php';
include_once 'PEAR/Registry.php';
include_once 'PEAR/Frontend/CLI.php';

$force = false;
$install_files = array();
array_shift($argv);
for ($i = 0; $i < sizeof($argv); $i++) {
    $arg = $argv[$i];
    $bn = basename($arg);
    if (ereg('package-(.*)\.xml$', $bn, $matches) ||
        ereg('([A-Za-z0-9_:]+)-.*\.(tar|tgz)$', $bn, $matches)) {
        $install_files[$matches[1]] = $arg;
    } elseif ($arg == '--force') {
        $force = true;
    } elseif ($arg == '-d') {
        $with_dir = $argv[$i+1];
        $i++;
    } elseif ($arg == '-b') {
        $bin_dir = $argv[$i+1];
        $i++;
    }
}

$config = &PEAR_Config::singleton();

// make sure we use only default values
$config_layers = $config->getLayers();
foreach ($config_layers as $layer) {
    if ($layer == 'default') continue;
    $config->removeLayer($layer);
}
$keys = $config->getKeys();
$config->set('verbose', 0, 'default');
// PEAR executables
if (!empty($bin_dir)) {
    $config->set('bin_dir', $bin_dir, 'default');
}
// User supplied a dir prefix
if (!empty($with_dir)) {
    $ds = DIRECTORY_SEPARATOR;
    $config->set('php_dir', $with_dir, 'default');
    $config->set('doc_dir', $with_dir . $ds . 'doc', 'default');
    $config->set('data_dir', $with_dir . $ds . 'data', 'default');
    $config->set('test_dir', $with_dir . $ds . 'test', 'default');
}
/* Print PEAR Conf (useful for debuging do NOT REMOVE)
sort($keys);
foreach ($keys as $key) {
    echo $config->getPrompt($key) . ": " . $config->get($key) . "\n";
}
exit;
// end print
//*/

$php_dir = $config->get('php_dir');
$options = array();
$install_root = getenv('INSTALL_ROOT');
if (!empty($install_root)) {
    $options['installroot'] = $install_root;
    $reg_dir = $install_root . $php_dir;
} else {
    $reg_dir = $php_dir;
}

$reg = &new PEAR_Registry($reg_dir);
$ui = &new PEAR_Frontend_CLI();
$installer = &new PEAR_Installer($ui);
//$installer->registry = &$reg; // This should be changed in Installer/Registry

foreach ($install_files as $package => $instfile) {
    if ($reg->packageExists($package)) {
        $info = $installer->infoFromAny($instfile);
        if (PEAR::isError($info)) {
            $ui->outputData(sprintf("[PEAR] %s: %s", $package, $info->getMessage()));
            continue;
        }
        $new_ver = $info['version'];
        $old_ver = $reg->packageInfo($package, 'version');
        if (version_compare($new_ver, $old_ver, 'gt')) {
            $options['upgrade'] = true;
            $err = $installer->install($instfile, $options);
            if (PEAR::isError($err)) {
                $ui->outputData(sprintf("[PEAR] %s: %s", $package, $err->getMessage()));
                continue;
            }
            $ui->outputData(sprintf("[PEAR] %-15s- upgraded:  %s", $package, $new_ver));
        } else {
            if ($force) {
                $options['force'] = true;
                $err = $installer->install($instfile, $options);
                if (PEAR::isError($err)) {
                    $ui->outputData(sprintf("[PEAR] %s: %s", $package, $err->getMessage()));
                    continue;
                }
                $ui->outputData(sprintf("[PEAR] %-15s- installed: %s", $package, $new_ver));
            } else {
                $ui->outputData(sprintf("[PEAR] %-15s- already installed: %s", $package, $old_ver));
            }
        }
    } else {
        $options['nodeps'] = true;
        $err = $installer->install($instfile, $options);
        if (PEAR::isError($err)) {
            $ui->outputData(sprintf("[PEAR] %s: %s", $package, $err->getMessage()));
            continue;
        }
        $new_ver = $reg->packageInfo($package, 'version');
        $ui->outputData(sprintf("[PEAR] %-15s- installed: %s", $package, $new_ver));
    }
    if ($package == 'PEAR') {
        if (is_file($ufile = $config->getConfFile('user'))) {
            $ui->outputData('Warning! a PEAR user config file already exists from ' .
                            'a previous PEAR installation at ' .
                            "'$ufile'. You may probably want to remove it.");
        }
        $config->set('verbose', 1, 'default');
        foreach ($config->getKeys() as $key) {
            $data[$key] = $config->get($key);
        }
        $cnf_file = $config->getConfFile('system');
        if (!empty($install_root)) {
            $cnf_file = $install_root . DIRECTORY_SEPARATOR . $cnf_file;
        }
        $config->writeConfigFile($cnf_file, 'system', $data);
        $ui->outputData('Wrote PEAR system config file at: ' . $cnf_file);
        $ui->outputData('You may want to add: ' . $config->get('php_dir') . ' to your php.ini include_path');
    }
}
?>
