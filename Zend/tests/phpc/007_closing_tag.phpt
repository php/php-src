--TEST--
.phpc: ?> in a .phpc file drops to inline output, mirroring .php semantics
--FILE--
<?php
$file = __DIR__ . '/' . basename(__FILE__, '.php') . '.phpc';
file_put_contents($file, "echo \"from-phpc\\n\";\n?>\nplain inline content\n<?php echo \"back\\n\";");
register_shutdown_function(fn() => @unlink($file));
require $file;
?>
--EXPECT--
from-phpc
plain inline content
back
