<?php

$reg = new PEAR_Registry($script_dir);
$installed = $reg->packageInfo();
$i = $j = 0;
heading("Installed packages:");
foreach ($installed as $package) {
    if ($i++ % 20 == 0) {
        if ($j++ > 0) {
            print "\n";
        }
        printf("%-20s %-10s %s\n",
               "Package", "Version", "State");
        print str_repeat("-", 75)."\n";
    }
    printf("%-20s %-10s %s\n", $package['package'],
           $package['version'], $package['release_state']);
}

?>