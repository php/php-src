<?php

$remote = new PEAR_Remote($config);
$result = $remote->call('package.listAll', 1);
$i = $j = 0;

if (PEAR::isError($result)) {
    die("\n\n   Error while executing command!\n\n");
}

heading("Available packages");
foreach ($result as $package => $info) {
    if ($i++ % 20 == 0) {
        if ($j++ > 0) {
            print "\n";
        }
        printf("%-20s %-10s %-15s %s\n",
               "Package", "Stable", "Lead", "Category");
        print str_repeat("-", 75)."\n";
    }
    if (empty($info['stable'])) {
        $info['stable'] = '(none)';
    }
    $stable = (string)$info['stable'];
    printf("%-20s %-10s %-15s %s\n", $info['name'],
           $info['stable'], $info['lead'], $info['category']);
}

?>