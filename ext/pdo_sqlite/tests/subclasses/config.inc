<?php

function getSpatialiteExtensionLocation() {
    $locations = [
        '/usr/lib/aarch64-linux-gnu/mod_spatialite.so',
        "/usr/lib/x86_64-linux-gnu/mod_spatialite.so"
    ];

    foreach ($locations as $location) {
        if (file_exists($location)) {
            return $location;
        }
    }

    return null;
}
