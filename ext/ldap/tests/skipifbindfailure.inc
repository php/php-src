<?php
require_once 'connect.inc';

if ($skip_on_bind_failure) {

    $link = ldap_connect($uri);
    ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
    if (!@ldap_bind($link, $user, $passwd))
        die(sprintf("skip Can't bind to LDAP Server - [%d] %s", ldap_errno($link), ldap_error($link)));

    ldap_unbind($link);
}

if (isset($require_vendor)) {
    ob_start();
    phpinfo(INFO_MODULES);
    $phpinfo = ob_get_clean();

    // Extract the LDAP section specifically
    if (preg_match('/^ldap\s*$(.*?)^[a-z_]+\s*$/ims', $phpinfo, $ldap_section_match)) {
        $ldap_section = $ldap_section_match[1];

        // Extract vendor info from the LDAP section only
        if (preg_match('/Vendor Name\s*=>\s*(.+)/i', $ldap_section, $name_match) &&
            preg_match('/Vendor Version\s*=>\s*(\d+)/i', $ldap_section, $version_match)) {

            $vendor_name = trim($name_match[1]);
            $vendor_version = (int)$version_match[1];

            // Check vendor name if specified
            if (isset($require_vendor['name']) && $vendor_name !== $require_vendor['name']) {
                die("skip Requires {$require_vendor['name']} (detected: $vendor_name)");
            }

            // Check minimum version if specified
            if (isset($require_vendor['min_version']) && $vendor_version < $require_vendor['min_version']) {
                die("skip Requires minimum version {$require_vendor['min_version']} (detected: $vendor_version)");
            }
        } else {
            die("skip Cannot determine LDAP vendor information");
        }
    } else {
        die("skip LDAP extension information not found");
    }
}
?>
