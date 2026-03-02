<?php

register_shutdown_function(function() {
    require 'path/to/an/unknown/file';
});
