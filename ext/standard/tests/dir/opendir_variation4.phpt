--TEST--
Test opendir() function : usage variations - different relative paths
--FILE--
<?php
/*
 * Test opendir() with different relative paths as $path argument
 */

echo "*** Testing opendir() : usage variation ***\n";

$base_dir_path = __DIR__ . '/opendir_variation4';
@mkdir($base_dir_path);

$level_one_dir_name = "level_one";
$level_one_dir_path = "$base_dir_path/$level_one_dir_name";

$level_two_dir_name = "level_two";
$level_two_dir_path = "$base_dir_path/$level_one_dir_name/$level_two_dir_name";

// create directories
mkdir($level_one_dir_path);
mkdir($level_two_dir_path);

echo "\n-- \$path = './level_one': --\n";
var_dump(chdir($base_dir_path));
var_dump($dh = opendir("./$level_one_dir_name"));
clean_dh($dh);

echo "\n-- \$path = 'level_one/level_two': --\n";
var_dump(chdir($base_dir_path));
var_dump($dh = opendir("$level_one_dir_name/$level_two_dir_name"));
clean_dh($dh);

echo "\n-- \$path = '..': --\n";
var_dump($dh = opendir('..'));
clean_dh($dh);

echo "\n-- \$path = 'level_two', '.': --\n";
var_dump(chdir($level_two_dir_path));
var_dump($dh = opendir('.'));
clean_dh($dh);

echo "\n-- \$path = '../': --\n";
var_dump($dh = opendir('../'));
clean_dh($dh);

echo "\n-- \$path = './': --\n";
var_dump(chdir($level_two_dir_path));
var_dump($dh = opendir('./'));
clean_dh($dh);

echo "\n-- \$path = '../../'level_one': --\n";
var_dump(chdir($level_two_dir_path));
var_dump($dh = opendir("../../$level_one_dir_name"));
clean_dh($dh);

/*
 * function to remove directory handle before re-using variable name in test
 * and to ensure directory is not in use at CLEAN section so can me removed
 */
function clean_dh($dh){
    if (is_resource($dh)) {
        closedir($dh);
    }
    unset($dh);
}
?>
--CLEAN--
<?php
$base_dir_path = __DIR__ . '/opendir_variation4';
rmdir("$base_dir_path/level_one/level_two");
rmdir("$base_dir_path/level_one");
rmdir($base_dir_path);
?>
--EXPECTF--
*** Testing opendir() : usage variation ***

-- $path = './level_one': --
bool(true)
resource(%d) of type (stream)

-- $path = 'level_one/level_two': --
bool(true)
resource(%d) of type (stream)

-- $path = '..': --
resource(%d) of type (stream)

-- $path = 'level_two', '.': --
bool(true)
resource(%d) of type (stream)

-- $path = '../': --
resource(%d) of type (stream)

-- $path = './': --
bool(true)
resource(%d) of type (stream)

-- $path = '../../'level_one': --
bool(true)
resource(%d) of type (stream)
