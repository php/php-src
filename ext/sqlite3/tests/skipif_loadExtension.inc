<?php

if (!method_exists(new SQLite3(':memory:'), 'loadExtension')) {
	die("skip if SQLITE_OMIT_LOAD_EXTENSION defined");
}
