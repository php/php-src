<?php

file_put_contents("{LOG_FILE}", var_export(getallheaders(), true));

echo <<<END
<?xml version="1.0" encoding="utf-8"?>
<schema>
</schema>
END;