<?php

include_once 'PEAR/Installer.php';
$pkgfile = $cmdargs[0];
$installer =& new PEAR_Installer($script_dir, $ext_dir, $doc_dir);
$installer->setErrorHandling(PEAR_ERROR_DIE,
                             basename($pkgfile) . ": %s\n");
$installer->debug = $verbose;
$install_options = array();
if ($command == 'upgrade') {
    $install_options['upgrade'] = true;
}
foreach ($cmdopts as $opt) {
    switch ($opt[0]) {
        case 'r':
            // This option is for use by rpm and other package
            // tools that can install files etc. by itself, but
            // still needs to register the package as installed in
            // PEAR's local registry.
            $install_options['register_only'] = true;
            break;
        case 'f':
            $install_options['force'] = true;
            break;
    }
}
if ($installer->install($pkgfile, $install_options, $config)) {
    print "install ok\n";
} else {
    print "install failed\n";
}

?>