<?php
# inline Reproducer class definition and segfaults will go away
require_once(__DIR__ . '/gh16829_2.inc');

# remove $someVar1\2 or $someVar3 and loop at the end of the file and segfaults will go away
$someVar2 = null;
$someVar1 = null;
$someVar3 = [];

for ($i = 0; $i < 10; $i++) {
    Reproducer::loops();
}

foreach ($someVar3 as $_) {
}
?>