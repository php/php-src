<?php
# PDO test framework utilities

if (getenv('PDOTEST_DSN') === false) {
    $common = '';
    $append = false;
    foreach(file(dirname($_SERVER['PHP_SELF']).'/common.phpt') as $line) {
        if ($append) {
            $common .= $line;
        } elseif (trim($line) == '--REDIRECTTEST--') {
            $append = true;
        }
    }
    $conf = eval($common);
    foreach($conf['ENV'] as $n=>$v) putenv("$n=$v");
}

class PDOTest {
    private static function getAttributes(string $environmentVariable): ?array {
        $attributes = getenv($environmentVariable);
        if (is_string($attributes) && strlen($attributes)) {
            return unserialize($attributes);
        }
        return null;
    }

    // create an instance of the PDO driver, based on
    // the current environment
    static function factory($classname = PDO::class, bool $useConnectMethod = false, ?array $attributes = null) {
        $dsn = getenv('PDOTEST_DSN');
        $user = getenv('PDOTEST_USER');
        $pass = getenv('PDOTEST_PASS');
        if ($attributes === null) {
            $attributes = self::getAttributes('PDOTEST_ATTR');
        }

        if ($user === false) $user = NULL;
        if ($pass === false) $pass = NULL;

        if ($useConnectMethod) {
            $db = $classname::connect($dsn, $user, $pass, $attributes);
        } else {
            $db = new $classname($dsn, $user, $pass, $attributes);
        }

        if (!$db) {
            die("Could not create PDO object (DSN=$dsn, user=$user)\n");
        }
        // Ignore errors about non-existent tables
        $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

        $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
        $db->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER);
        $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
        return $db;
    }

    private static function getSkipCacheFile(): ?string {
        $directory = getenv('TEST_PHP_SHARED_CACHE_DIR');
        if (!is_string($directory) || !is_dir($directory)) {
            return null;
        }

        $configuration = [
            getenv('PDOTEST_DSN'),
            getenv('PDOTEST_USER'),
            getenv('PDOTEST_PASS'),
            getenv('PDOTEST_ATTR'),
            getenv('PDOTEST_SKIP_ATTR'),
        ];
        return $directory . DIRECTORY_SEPARATOR . 'pdo-' . hash('sha256', serialize($configuration));
    }

    static function factoryForSkip() {
        $attributes = self::getAttributes('PDOTEST_ATTR');
        $skipAttributes = self::getAttributes('PDOTEST_SKIP_ATTR');
        if ($skipAttributes !== null) {
            $attributes = $skipAttributes + ($attributes ?? []);
        }
        return PDOTest::factory(PDO::class, false, $attributes);
    }

    private static function connectOrSkip(): void {
        try {
            self::factoryForSkip();
        } catch (PDOException $e) {
            die("skip " . $e->getMessage());
        }
    }

    static function skip() {
        $cacheFile = self::getSkipCacheFile();
        if ($cacheFile === null) {
            self::connectOrSkip();
            return;
        }

        $cache = @fopen($cacheFile, 'c+');
        if ($cache === false || !flock($cache, LOCK_EX)) {
            if (is_resource($cache)) {
                fclose($cache);
            }
            self::connectOrSkip();
            return;
        }

        $cached = stream_get_contents($cache);
        $reason = $cached !== '' ? $cached : null;
        if ($reason === null) {
            // Only failures are shared; successful checks still create their own connection.
            try {
                self::factoryForSkip();
            } catch (PDOException $e) {
                $reason = $e->getMessage();
                rewind($cache);
                ftruncate($cache, 0);
                fwrite($cache, $reason);
                fflush($cache);
            }
        }

        flock($cache, LOCK_UN);
        fclose($cache);
        if (is_string($reason)) {
            die("skip $reason");
        }
    }

    static function test_factory($file, $classname = PDO::class, bool $useConnectMethod = false) {
        $config = self::get_config($file);
        foreach ($config['ENV'] as $k => $v) {
            putenv("$k=$v");
        }
        return self::factory($classname, $useConnectMethod);
    }

    static function get_config($file) {
        $data = file_get_contents($file);
        $data = preg_replace('/^.*--REDIRECTTEST--/s', '', $data);
        $config = eval($data);

        return $config;
    }

    public static function dropTableIfExists(PDO $db, string $tableName): void {
        if (getenv('PDOTEST_DSN') === "sqlite::memory:") {
            return;
        }

        match ($db->getAttribute(PDO::ATTR_DRIVER_NAME)) {
            "oci" => $db->exec(<<<SQL
                 BEGIN
                     EXECUTE IMMEDIATE 'DROP TABLE $tableName';
                 EXCEPTION
                     WHEN OTHERS THEN
                         IF SQLCODE != -942 THEN
                             RAISE;
                         END IF;
                    END;
                SQL
            ),
            "firebird" => @$db->exec("DROP TABLE $tableName"),
            default => $db->exec("DROP TABLE IF EXISTS $tableName"),
        };
    }
}
/** See https://stackoverflow.com/a/3732466 */
function get_dummy_sql_request(): string
{
    $dsn = getenv('PDOTEST_DSN');

    // Firebird: https://www.firebirdfaq.org/faq30/
    if (str_starts_with($dsn, 'firebird')) {
        return "SELECT FIRST 0 FROM (VALUES ('Hello world')";
    }

    // Oracle
    if (str_starts_with($dsn, 'oci')) {
        return 'SELECT 1 FROM DUAL';
    }

    return 'SELECT 1';
}
